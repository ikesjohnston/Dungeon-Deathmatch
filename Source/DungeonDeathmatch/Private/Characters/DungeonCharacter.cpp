// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacter.h"
#include "DungeonDeathmatch.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "DungeonPlayerController.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "DungeonAbilitySystemComponent.h"
#include "DungeonAttributeSet.h"
#include "DungeonGameplayAbility.h"
#include <GameplayEffect.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <WidgetComponent.h>
#include "Interactables/Interactable.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include <Private/KismetTraceUtils.h>

// Console command for logging melee combo states
static int32 LogCombos = 0;
FAutoConsoleVariableRef CVARLogCombos(
	TEXT("Dungeon.LogCombos"),
	LogCombos,
	TEXT("Log melee combo states"),
	ECVF_Cheat);

ADungeonCharacter::ADungeonCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	//GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Set up fist colliders and overlap events for unarmed combat. 
	// Collision should be off by default, it will get toggled on and off during animations.
	FistColliderLeft = CreateDefaultSubobject<USphereComponent>(TEXT("FistColliderLeft"));
	FistColliderLeft->SetupAttachment(GetMesh(), "HandLeft");
	FistColliderLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FistColliderLeft->OnComponentBeginOverlap.AddDynamic(this, &ADungeonCharacter::OnFistColliderLeftBeginOverlap);

	FistColliderRight = CreateDefaultSubobject<USphereComponent>(TEXT("FistColliderRight"));
	FistColliderRight->SetupAttachment(GetMesh(), "HandRight");
	FistColliderRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FistColliderRight->OnComponentBeginOverlap.AddDynamic(this, &ADungeonCharacter::OnFistColliderRightBeginOverlap);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	// Initialize Health Plate widget.
	HealthPlateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Plate Widget"));
	HealthPlateWidget->SetupAttachment(GetCapsuleComponent());
	HealthPlateWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthPlateWidget->bOwnerNoSee = true;
	HealthPlateWidget->bOnlyOwnerSee = false;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UDungeonAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UDungeonAttributeSet>(TEXT("AttributeSet"));

	bAbilitiesInitialized = false;

	bIsMeleeComboReady = true;
	CurrentMeleeComboState = -1;

	bCanLook = true;
}

void ADungeonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonCharacter, CurrentMeleeComboState);
	DOREPLIFETIME(ADungeonCharacter, bIsMeleeComboReady);
}

// Called when the game starts or when spawned
void ADungeonCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		DefaultWalkingDeceleration = MovementComp->BrakingDecelerationWalking;
		DefaultGroundFriction = MovementComp->GroundFriction;
	}

	if(AbilitySystemComponent)
	{
		if (HasAuthority())
		{
			if (StartSprintAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StartSprintAbility.GetDefaultObject()), 1, 0));
			}
			if (StopSprintAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StopSprintAbility.GetDefaultObject()), 1, 0));
			}
			if (JumpAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(JumpAbility.GetDefaultObject()), 1, 0));
			}
			if (CrouchAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(CrouchAbility.GetDefaultObject()), 1, 0));
			}
			if (RollAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(RollAbility.GetDefaultObject()), 1, 0));
			}
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}

	// Only show health plates on enemy characters
	if (IsLocallyControlled())
	{
		HealthPlateWidget->SetVisibility(false);
	}
}

void ADungeonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

// Called to bind functionality to input
void ADungeonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement Inputs
	PlayerInputComponent->BindAxis("MoveForward", this, &ADungeonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADungeonCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ADungeonCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ADungeonCharacter::LookRight);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADungeonCharacter::OnSprintKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADungeonCharacter::OnSprintKeyReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADungeonCharacter::OnCrouchKeyPressed);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADungeonCharacter::OnJumpKeyPressed);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ADungeonCharacter::OnRollKeyPressed);

	// Action Inputs
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADungeonCharacter::OnInteractKeyPressed);
	PlayerInputComponent->BindAction("Sheathe", IE_Pressed, this, &ADungeonCharacter::OnSheatheKeyPressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ADungeonCharacter::OnAttackKeyPressed);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ADungeonCharacter::OnBlockKeyPressed);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ADungeonCharacter::OnBlockKeyReleased);

	// Menu Inputs
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ADungeonCharacter::OnInventoryKeyPressed);
	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &ADungeonCharacter::OnEscapeKeyPressed);
	PlayerInputComponent->BindAction("UseInventoryItem", IE_Pressed, this, &ADungeonCharacter::OnUseInventoryItemKeyPressed);
	PlayerInputComponent->BindAction("DropInventoryItem", IE_Pressed, this, &ADungeonCharacter::OnDropInventoryItemKeyPressed);

	//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));

}

UInventoryComponent* ADungeonCharacter::GetInventoryComponent()
{
	return InventoryComponent;
}

UEquipmentComponent* ADungeonCharacter::GetEquipmentComponent()
{
	return EquipmentComponent;
}

UAbilitySystemComponent* ADungeonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADungeonCharacter::GiveAbility(TSubclassOf<UGameplayAbility> Ability)
{
	if (Role == ROLE_Authority)
	{
		if (AbilitySystemComponent && Ability)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1));
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ADungeonCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UDungeonGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}
		if (UnarmedMeleeComboAbilities.Num() > 0)
		{
			CurrentMeleeComboState = 0;
			for (TSubclassOf<UDungeonGameplayAbility>& ComboAbility : UnarmedMeleeComboAbilities)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ComboAbility, GetCharacterLevel(), INDEX_NONE, this));
			}
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}

void ADungeonCharacter::RemoveStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && bAbilitiesInitialized)
	{
		// Remove any abilities added from a previous call
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && GameplayAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		// Do in two passes so the removal happens after we have the full list
		for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
		{
			AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		}

		// Remove all of the passive gameplay effects that were applied by this character
		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);

		bAbilitiesInitialized = false;
	}
}

float ADungeonCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float ADungeonCharacter::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float ADungeonCharacter::GetMana() const
{
	return AttributeSet->GetMana();
}

float ADungeonCharacter::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

float ADungeonCharacter::GetStamina() const
{
	return AttributeSet->GetStamina();
}

float ADungeonCharacter::GetMaxStamina() const
{
	return AttributeSet->GetMaxStamina();
}

float ADungeonCharacter::GetStaminaRegen() const
{
	return AttributeSet->GetStaminaRegen();
}

float ADungeonCharacter::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

int32 ADungeonCharacter::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool ADungeonCharacter::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		// Our level changed so we need to refresh abilities
		RemoveStartupGameplayAbilities();
		CharacterLevel = NewLevel;
		AddStartupGameplayAbilities();

		return true;
	}
	return false;
}

void ADungeonCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorPawn, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorPawn, DamageCauser);
}

void ADungeonCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (Role == ROLE_Authority && GetHealth() <= 0)
	{
		Multicast_OnDeath();
	}

	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void ADungeonCharacter::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnManaChanged(DeltaValue, EventTags);
	}
}

void ADungeonCharacter::HandleStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnStaminaChanged(DeltaValue, EventTags);
	}
}

void ADungeonCharacter::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// Update the character movement's walk speed
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();

	if (bAbilitiesInitialized)
	{
		OnMoveSpeedChanged(DeltaValue, EventTags);
	}
}

void ADungeonCharacter::Multicast_OnDeath_Implementation()
{
	// Stop player input
	DetachFromControllerPendingDestroy();
	OnDeath();

	// Ragdoll the character's mesh
	USkeletalMeshComponent* Mesh = GetMesh();
	Mesh->SetCollisionObjectType(ECC_PhysicsBody);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();

	// TODO: Add a force to the mesh based on the location and type of hit that killed the character

	// Disable capsule collision so we can walk over ragdoll
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetEnableGravity(false);
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Disable health plates on dead characters
	HealthPlateWidget->Deactivate();
	HealthPlateWidget->SetVisibility(false);
}

void ADungeonCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ADungeonCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ADungeonCharacter::LookRight(float YawInput)
{
	if (bCanLook)
	{
		AddControllerYawInput(YawInput);
	}
}

void ADungeonCharacter::LookUp(float PitchInput)
{
	if (bCanLook)
	{
		AddControllerPitchInput(PitchInput);
	}
}

void ADungeonCharacter::SetCanLook(bool CanLook)
{
	bCanLook = CanLook;
}

void ADungeonCharacter::OnSprintKeyPressed()
{
	if (StartSprintAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StartSprintAbility);
	}
}

void ADungeonCharacter::OnSprintKeyReleased()
{
	if (StopSprintAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StopSprintAbility);
	}
}

void ADungeonCharacter::OnJumpKeyPressed()
{
	if (JumpAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(JumpAbility);
	}
}

void ADungeonCharacter::OnCrouchKeyPressed()
{
	if (CrouchAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(CrouchAbility);
	}
}

void ADungeonCharacter::OnRollKeyPressed()
{
	if (RollAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(RollAbility);
	}
}

void ADungeonCharacter::OnInteractKeyPressed()
{
	Server_Interact();
}

void ADungeonCharacter::Server_Interact_Implementation()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		AInteractable* FocusedInteractable = PlayerController->GetFocusedInteractable();
		if (FocusedInteractable)
		{
			AItem* Item = Cast<AItem>(FocusedInteractable);
			if (Item)
			{
				Server_TryPickUpItem(Item);
			}
			else
			{
				FocusedInteractable->Server_OnInteract(this);
			}
		}
	}
}

bool ADungeonCharacter::Server_Interact_Validate()
{
	return true;
}

void ADungeonCharacter::Server_TryPickUpItem_Implementation(AItem* Item)
{
	bool WasPickedUp = InventoryComponent->TryAddItem(Item);
}

bool ADungeonCharacter::Server_TryPickUpItem_Validate(AItem* Item)
{
	return true;
}

void ADungeonCharacter::Server_TryDropItem_Implementation(AItem* Item)
{
	bool WasDropped = InventoryComponent->TryRemoveItem(Item);
}

bool ADungeonCharacter::Server_TryDropItem_Validate(AItem* Item)
{
	return true;
}

void ADungeonCharacter::OnSheatheKeyPressed()
{
	/*if (CombatState == ECombatState::WeaponSheathed)
		UnsheatheWeapon();
	else if (CombatState == ECombatState::WeaponReady)
		SheatheWeapon();*/
}

void ADungeonCharacter::Server_SheatheWeapon_Implementation()
{
	/*AItem* Item = EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::MainHand);

	if (Item)
	{
		CombatState = ECombatState::SheathingWeapon;
	}*/
}

bool ADungeonCharacter::Server_SheatheWeapon_Validate()
{
	return true;
}

void ADungeonCharacter::Server_UnsheatheWeapon_Implementation()
{
	/*AItem* Item = EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::MainHand);

	if (Item)
	{
		CombatState = ECombatState::UnsheathingWeapon;
	}*/
}

bool ADungeonCharacter::Server_UnsheatheWeapon_Validate()
{
	return true;
}

void ADungeonCharacter::OnAttackKeyPressed()
{
	Server_Attack();
}

bool ADungeonCharacter::CanAttack()
{
	if (!bIsMeleeComboReady)
		return false;

	return true;
}

void ADungeonCharacter::Server_Attack_Implementation()
{
	if (CanAttack() && CurrentMeleeComboState < UnarmedMeleeComboAbilities.Num())
	{
		TSubclassOf<UDungeonGameplayAbility> ComboAbility = UnarmedMeleeComboAbilities[CurrentMeleeComboState];
		if (AbilitySystemComponent->TryActivateAbilityByClass(ComboAbility))
		{
			bIsMeleeComboReady = false;
		};
	}
}

bool ADungeonCharacter::Server_Attack_Validate()
{
	return true;
}

void ADungeonCharacter::Server_IncreaseMeleeComboState_Implementation()
{
	CurrentMeleeComboState++;
	Server_SetMeleeComboReady(true);
	if (LogCombos)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::Server_IncreaseCombo - Melee combo count increased to %d for %s"), CurrentMeleeComboState, *GetName());
	}
	if (CurrentMeleeComboState > (UnarmedMeleeComboAbilities.Num() - 1))
	{
		if (LogCombos)
		{
			UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::Server_IncreaseCombo - Max melee combo count reached for %s"), *GetName());
		}
		Server_ResetMeleeComboState();
	}
}

bool ADungeonCharacter::Server_IncreaseMeleeComboState_Validate()
{
	return true;
}

void ADungeonCharacter::Server_SetMeleeComboReady_Implementation(bool ComboReady)
{
	bIsMeleeComboReady = ComboReady;
}

bool ADungeonCharacter::Server_SetMeleeComboReady_Validate(bool ComboReady)
{
	return true;
}

void ADungeonCharacter::Server_BeginMeleeComboEndTimer_Implementation(float TimeToComboEnd)
{
	if (LogCombos)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::Server_BeginMeleeComboEndTimer - Starting melee combo end timer with %f seconds for %s"), TimeToComboEnd, *GetName());
	}
	GetWorldTimerManager().SetTimer(MeleeComboEndTimer, this, &ADungeonCharacter::Server_ResetMeleeComboState, TimeToComboEnd, false);
}

bool ADungeonCharacter::Server_BeginMeleeComboEndTimer_Validate(float TimeToComboEnd)
{
	return true;
}

void ADungeonCharacter::Server_ResetMeleeComboState_Implementation()
{
	CurrentMeleeComboState = 0;
	if (LogCombos)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::Server_ResetCombo - Combo count reset for %s"), *GetName());
	}
}

bool ADungeonCharacter::Server_ResetMeleeComboState_Validate()
{
	return true;
}

void ADungeonCharacter::Server_CancelAttack_Implementation()
{

}

bool ADungeonCharacter::Server_CancelAttack_Validate()
{
	return true;
}

UAnimationProfile* ADungeonCharacter::GetAnimationProfile()
{
	return AnimationProfile;
}

USphereComponent* ADungeonCharacter::GetLeftFistCollider()
{
	return FistColliderLeft;
}

USphereComponent* ADungeonCharacter::GetRightFistCollider()
{
	return FistColliderRight;
}

void ADungeonCharacter::OnFistColliderLeftBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority)
	{
		if (OtherActor == this)
		{
			return;
		}
		SendUnarmedMeleeHitEvent(OtherActor);
	}
}

void ADungeonCharacter::OnFistColliderRightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority)
	{
		if (OtherActor == this)
		{
			return;
		}
		SendUnarmedMeleeHitEvent(OtherActor);
	}
}

void ADungeonCharacter::SendUnarmedMeleeHitEvent(AActor* HitActor)
{
	if (Role == ROLE_Authority)
	{
		FGameplayEventData HitEventData = FGameplayEventData();
		HitEventData.EventTag = UnarmedMeleeHitEventTag;
		HitEventData.Instigator = this;
		HitEventData.Target = HitActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UnarmedMeleeHitEventTag, HitEventData);
	}
}

void ADungeonCharacter::OnBlockKeyPressed()
{

}

void ADungeonCharacter::OnBlockKeyReleased()
{

}

void ADungeonCharacter::OnInventoryKeyPressed()
{
	// Pass input to controller for processing
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnInventoryKeyPressed();
	}
}

void ADungeonCharacter::OnEscapeKeyPressed()
{
	// Pass input to controller for processing
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnEscapeKeyPressed();
	}
}

void ADungeonCharacter::OnUseInventoryItemKeyPressed()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnUseInventoryItemKeyPressed();
	}
}

void ADungeonCharacter::OnDropInventoryItemKeyPressed()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnDropInventoryItemKeyPressed();
	}
}
