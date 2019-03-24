// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacter.h"
#include "DungeonDeathmatch.h"
#include "Interactables/Interactable.h"
#include "Armor.h"
#include "Weapon.h"
#include "CharacterRenderCapture2D.h"
#include "ModularCharacterMeshComponent.h"
#include "DungeonPlayerController.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "DungeonAbilitySystemComponent.h"
#include "DungeonAttributeSet.h"
#include "DungeonGameplayAbility.h"
#include "PlayerCombatComponent.h"
#include "RenderCaptureComponent.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <GameplayEffect.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <WidgetComponent.h>
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

#define CARDINAL_MOVEMENT_FORWARD_MAX 22.5f
#define CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX 67.5f
#define CARDINAL_MOVEMENT_RIGHT_MAX 112.5f
#define CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX 157.5f
#define CARDINAL_MOVEMENT_BACKWARD 180.0f

#define MOVEMENT_SPEED_ATTRIBUTE_DIVISOR 100.0f

ADungeonCharacter::ADungeonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	VitalsPlateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("VitalsPlateWidget"));
	VitalsPlateWidget->SetupAttachment(GetCapsuleComponent());
	VitalsPlateWidget->SetWidgetSpace(EWidgetSpace::Screen);
	VitalsPlateWidget->bOwnerNoSee = true;
	VitalsPlateWidget->bOnlyOwnerSee = false;

	ModularCharacterMesh = CreateDefaultSubobject<UModularCharacterMeshComponent>(TEXT("ModularCharacterMesh"));

	RenderCaptureComponent = CreateDefaultSubobject<URenderCaptureComponent>(TEXT("RenderCaptureComponent"));

	PlayerCombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));

	ItemDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation"));
	ItemDropLocation->SetupAttachment(GetMesh());

	// Initialize movement systems
	BaseStandingMovementSpeed = 400.0f;
	GetCharacterMovement()->MaxWalkSpeed = BaseStandingMovementSpeed;
	BaseCrouchedMovementSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = BaseCrouchedMovementSpeed;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UDungeonAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UDungeonAttributeSet>(TEXT("AttributeSet"));

	bAbilitiesInitialized = false;

	// Initialize Inventory & Equipment systems
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	SocketNameConsumableOne = "ConsumableOne";
	SocketNameConsumableTwo = "ConsumableTwo";

	bIsMovementInputEnabled = true;
	bIsCameraInputEnabled = true;

	MovementDirectionYawClampMin = -179.0f;
	MovementDirectionYawClampMax = 180.0f;

	AimYawTurnStart = 60;
	AimYawTurnStop = 5;
	AimYawClampMin = -120;
	AimYawClampMax = 120;
	AimPitchClampMin = -90;
	AimPitchClampMax = 90;

	DropEjectionForce = 20000;
}

void ADungeonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonCharacter, AimYaw);
	DOREPLIFETIME(ADungeonCharacter, AimPitch);
	DOREPLIFETIME(ADungeonCharacter, bIsReorientingBody);
	DOREPLIFETIME(ADungeonCharacter, bIsManuallyFreeLooking);
	DOREPLIFETIME(ADungeonCharacter, bIsAutoFreeLooking);
	DOREPLIFETIME(ADungeonCharacter, bIsMovementInputEnabled);
	DOREPLIFETIME(ADungeonCharacter, bIsJumping);
}

void ADungeonCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	ModularCharacterMesh->Initialize();
}

// Called when the game starts or when spawned
void ADungeonCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseStandingMovementSpeed;
	BaseCrouchedMovementSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = BaseCrouchedMovementSpeed;

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
			if (StartFreeLookAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StartFreeLookAbility.GetDefaultObject()), 1, 0));
			}
			if (StopFreeLookAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StopFreeLookAbility.GetDefaultObject()), 1, 0));
			}
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();

		if (IsLocallyControlled())
		{
			// Only show health plates on enemy characters
			VitalsPlateWidget->SetVisibility(false);
		}
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
	PlayerInputComponent->BindAxis("Move Forward", this, &ADungeonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right", this, &ADungeonCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up", this, &ADungeonCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ADungeonCharacter::LookRight);
	PlayerInputComponent->BindAction("Free Look", IE_Pressed, this, &ADungeonCharacter::OnFreeLookKeyPressed);
	PlayerInputComponent->BindAction("Free Look", IE_Released, this, &ADungeonCharacter::OnFreeLookKeyReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADungeonCharacter::OnSprintKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADungeonCharacter::OnSprintKeyReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADungeonCharacter::OnCrouchKeyPressed);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADungeonCharacter::OnJumpKeyPressed);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ADungeonCharacter::OnRollKeyPressed);

	// Action Inputs
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADungeonCharacter::OnInteractKeyPressed);
	PlayerInputComponent->BindAction("Switch Loadout", IE_Pressed, this, &ADungeonCharacter::OnLoadoutSwitchKeyPressed);
	PlayerInputComponent->BindAction("Sheathe Weapon", IE_Pressed, this, &ADungeonCharacter::OnSheatheKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Attack", IE_Pressed, this, &ADungeonCharacter::OnMainHandAttackKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Attack", IE_Released, this, &ADungeonCharacter::OnMainHandAttackKeyReleased);
	PlayerInputComponent->BindAction("Main Hand Alt. Attack", IE_Pressed, this, &ADungeonCharacter::OnMainHandAltAttackKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Alt. Attack", IE_Released, this, &ADungeonCharacter::OnMainHandAltAttackKeyReleased);
	PlayerInputComponent->BindAction("Off Hand Attack", IE_Pressed, this, &ADungeonCharacter::OnOffHandAttackKeyPressed);
	PlayerInputComponent->BindAction("Off Hand Attack", IE_Released, this, &ADungeonCharacter::OnOffHandAttackKeyReleased);
	PlayerInputComponent->BindAction("Off Hand Alt. Attack", IE_Pressed, this, &ADungeonCharacter::OnOffHandAltAttackKeyPressed);
	PlayerInputComponent->BindAction("Off Hand Alt. Attack", IE_Released, this, &ADungeonCharacter::OnOffHandAltAttackKeyReleased);

	// Menu Inputs
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ADungeonCharacter::OnInventoryKeyPressed);
	PlayerInputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ADungeonCharacter::OnEscapeKeyPressed);

	//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

void ADungeonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Have server calculate aim offset
	// TODO: This will need to be updated to use client prediction for smoothness at some point, with some defined error acception range
	if (Role == ROLE_Authority)
	{
		CalculateAimRotation();
	}

	// Set jumping back to false after hitting the ground, used by animation instances
	if (bIsJumping && !GetCharacterMovement()->IsFalling())
	{
		SetIsJumping(false);
	}
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

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : StartingGameplayEffects)
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
	VitalsPlateWidget->Deactivate();
	VitalsPlateWidget->SetVisibility(false);
}

float ADungeonCharacter::GetHealth() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetHealth - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetHealth();
}

float ADungeonCharacter::GetMaxHealth() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMaxHealth - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMaxHealth();
}

float ADungeonCharacter::GetMana() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMana - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMana();
}

float ADungeonCharacter::GetMaxMana() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMaxMana - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMaxMana();
}

float ADungeonCharacter::GetStamina() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetStamina - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetStamina();
}

float ADungeonCharacter::GetMaxStamina() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMaxStamina - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMaxStamina();
}

float ADungeonCharacter::GetStaminaRegen() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetStaminaRegen - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetStaminaRegen();
}

float ADungeonCharacter::GetMovementSpeed() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMovementSpeed - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMovementSpeed();
}

float ADungeonCharacter::GetMovementSpeedMultiplier() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Log, TEXT("DungeonCharacter::GetMovementSpeedMultiplier - No AttributeSet set for %s"), *GetName());
		return 0;
	}
	return AttributeSet->GetMovementSpeedMultiplier();
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

void ADungeonCharacter::HandleMovementSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// Update the character movement's walk speed
	GetCharacterMovement()->MaxWalkSpeed = BaseStandingMovementSpeed * ((GetMovementSpeed() * GetMovementSpeedMultiplier()) / MOVEMENT_SPEED_ATTRIBUTE_DIVISOR);
	GetCharacterMovement()->MaxWalkSpeedCrouched = BaseCrouchedMovementSpeed * ((GetMovementSpeed() * GetMovementSpeedMultiplier()) / MOVEMENT_SPEED_ATTRIBUTE_DIVISOR);

	if (bAbilitiesInitialized)
	{
		OnMovementSpeedChanged(DeltaValue, EventTags);
	}
}

void ADungeonCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
}

bool ADungeonCharacter::RemoveGameplayTag(FGameplayTag Tag)
{
	FGameplayTagContainer TagContainer;
	GetOwnedGameplayTags(TagContainer);
	return TagContainer.RemoveTag(Tag);
}

bool ADungeonCharacter::GetIsMovementInputEnabled()
{
	return bIsMovementInputEnabled;
}

void ADungeonCharacter::SetIsMovementInputEnabled(bool IsMovementInputEnabled)
{
	bIsMovementInputEnabled = IsMovementInputEnabled;
}

void ADungeonCharacter::SetIsCameraInputEnabled(bool CanLook)
{
	bIsCameraInputEnabled = CanLook;
}

void ADungeonCharacter::MoveForward(float Value)
{
	if (bIsMovementInputEnabled)
	{
		AddMovementInput(GetActorForwardVector() * Value);
	}
}

void ADungeonCharacter::MoveRight(float Value)
{
	if (bIsMovementInputEnabled)
	{
		AddMovementInput(GetActorRightVector() * Value);
	}
}

void ADungeonCharacter::LookRight(float YawInput)
{
	if (bIsCameraInputEnabled)
	{
		AddControllerYawInput(YawInput);
	}
}

void ADungeonCharacter::LookUp(float PitchInput)
{
	if (bIsCameraInputEnabled)
	{
		AddControllerPitchInput(PitchInput);
	}
}

void ADungeonCharacter::Jump()
{
	if (bIsMovementInputEnabled)
	{
		Super::Jump();

		SetIsJumping(true);
	}
}

void ADungeonCharacter::OnJumpKeyPressed()
{
	if (JumpAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(JumpAbility);
	}
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

void ADungeonCharacter::OnFreeLookKeyPressed()
{
	if (StartFreeLookAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StartFreeLookAbility);
	}
}

void ADungeonCharacter::OnFreeLookKeyReleased()
{
	if (StopFreeLookAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StopFreeLookAbility);
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
	Interact();
}

void ADungeonCharacter::OnLoadoutSwitchKeyPressed()
{
	PlayerCombatComponent->SwitchLoadout();
}

void ADungeonCharacter::OnSheatheKeyPressed()
{
	PlayerCombatComponent->ToggleWeaponSheathe();
}

void ADungeonCharacter::OnMainHandAttackKeyPressed()
{
	PlayerCombatComponent->PerformMainHandAttack();
}

void ADungeonCharacter::OnMainHandAttackKeyReleased()
{

}

void ADungeonCharacter::OnMainHandAltAttackKeyPressed()
{
	PlayerCombatComponent->PerformMainHandAltAttack();
}

void ADungeonCharacter::OnMainHandAltAttackKeyReleased()
{

}

void ADungeonCharacter::OnOffHandAttackKeyPressed()
{
	PlayerCombatComponent->PerformOffHandAttack();
}

void ADungeonCharacter::OnOffHandAttackKeyReleased()
{

}

void ADungeonCharacter::OnOffHandAltAttackKeyPressed()
{
	PlayerCombatComponent->PerformOffHandAltAttack();
}

void ADungeonCharacter::OnOffHandAltAttackKeyReleased()
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

void ADungeonCharacter::Interact()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		AActor* FocusedInteractable = PlayerController->GetFocusedInteractable();
		if (FocusedInteractable)
		{
			IInteractable* InteractableInterface = Cast<IInteractable>(FocusedInteractable);
			if (InteractableInterface)
			{
				InteractableInterface->Execute_OnInteract(FocusedInteractable, this);
			}
		}
	}
}

FVector ADungeonCharacter::GetMovementVelocity()
{
	FVector Velocity;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		Velocity = MovementComp->Velocity;
	}
	else
	{
		Velocity = FVector(0, 0, 0);
	}

	return Velocity;
}

float ADungeonCharacter::GetMovementDirection()
{
	float MovementDirection = 0.0f;

	FVector Velocity = GetMovementVelocity();
	if (Velocity.Size() > 0)
	{
		FRotator ActorRotation = GetActorRotation();
		FVector LocalVelocity = ActorRotation.UnrotateVector(Velocity);
		MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	}
	else
	{
		// If standing still, the aim direction should act as the move direction, this is used for rolling
		// If free aiming, however, move direction should be forward
		if (GetIsFreeLooking())
		{
			MovementDirection = 0;
		}
		else
		{
			FRotator DeltaRotation = GetControlRotation() - GetActorRotation();
			MovementDirection = FMath::ClampAngle(DeltaRotation.Yaw, MovementDirectionYawClampMin, MovementDirectionYawClampMax);
		}
	}

	return MovementDirection;
}

ECardinalMovementDirection ADungeonCharacter::GetCardinalMovementDirection()
{
	float MovementDirection = GetMovementDirection();

	if (MovementDirection >= -CARDINAL_MOVEMENT_FORWARD_MAX && MovementDirection <= CARDINAL_MOVEMENT_FORWARD_MAX)
	{
		return ECardinalMovementDirection::Forward;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_FORWARD_MAX && MovementDirection <= CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::ForwardRight;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_RIGHT_MAX)
	{
		return ECardinalMovementDirection::Right;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::BackwardRight;
	}
	else if ((MovementDirection > CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_BACKWARD) ||
		(MovementDirection >= -CARDINAL_MOVEMENT_BACKWARD && MovementDirection < -CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX))
	{
		return ECardinalMovementDirection::Backward;
	}
	else if (MovementDirection >= -CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX && MovementDirection < -CARDINAL_MOVEMENT_RIGHT_MAX)
	{
		return ECardinalMovementDirection::BackwardLeft;
	}
	else if (MovementDirection >= -CARDINAL_MOVEMENT_RIGHT_MAX && MovementDirection < -CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::Left;
	}
	else
	{
		return ECardinalMovementDirection::ForwardLeft;
	}
}

bool ADungeonCharacter::GetIsJumping()
{
	return bIsJumping;
}

void ADungeonCharacter::SetIsJumping(bool IsJumping)
{
	bIsJumping = IsJumping;
}

bool ADungeonCharacter::GetIsFreeLooking()
{
	bool IsFreeLooking = (bIsManuallyFreeLooking || bIsAutoFreeLooking);
	return IsFreeLooking;
}

void ADungeonCharacter::SetIsManuallyFreeLooking(bool IsManuallyFreeLooking)
{
	bIsManuallyFreeLooking = IsManuallyFreeLooking;
	UseControllerDesiredRotation(!(bIsManuallyFreeLooking || bIsAutoFreeLooking));
}

void ADungeonCharacter::SetIsAutoFreeLooking(bool IsAutoFreeLooking)
{
	bIsAutoFreeLooking = IsAutoFreeLooking;
	UseControllerDesiredRotation(!(bIsManuallyFreeLooking || bIsAutoFreeLooking));
}

bool ADungeonCharacter::GetIsReorientingBody()
{
	return bIsReorientingBody;
}

UBlendSpace* ADungeonCharacter::GetDefaultStandingMovementBlendSpace()
{
	return DefaultStandingMovementBlendSpace;
}

UBlendSpace* ADungeonCharacter::GetDefaultCrouchingMovementBlendSpace()
{
	return DefaultCrouchingMovementBlendSpace;
}

UAnimSequence* ADungeonCharacter::GetDefaultJumpingAnimation()
{
	return DefaultJumpingAnimation;
}

UBlendSpace1D* ADungeonCharacter::GetDefaultFallingBlendSpace()
{
	return DefaultFallingBlendSpace;
}

UBlendSpace* ADungeonCharacter::GetDefaultLandingBlendSpace()
{
	return DefaultLandingBlendSpace;
}

UBlendSpace* ADungeonCharacter::GetCombatStandingMovementBlendSpace()
{
	if (PlayerCombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

		if (ActiveLoadout.MainHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatStandingMovementBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}
		else if (ActiveLoadout.OffHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatStandingMovementBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}

		ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
		UBlendSpace** BlendSpacePtr = CombatStandingMovementBlendSpaceMap.Find(LoadoutType);
		if (BlendSpacePtr)
		{
			return *BlendSpacePtr;
		}
	}

	return DefaultStandingMovementBlendSpace;
}

UBlendSpace* ADungeonCharacter::GetCombatCrouchingMovementBlendSpace()
{
	if (PlayerCombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

		if (ActiveLoadout.MainHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatCrouchingMovementBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}
		else if (ActiveLoadout.OffHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatCrouchingMovementBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}

		ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
		UBlendSpace** BlendSpacePtr = CombatCrouchingMovementBlendSpaceMap.Find(LoadoutType);
		if (BlendSpacePtr)
		{
			return *BlendSpacePtr;
		}
	}

	return DefaultCrouchingMovementBlendSpace;
}

UAnimSequence* ADungeonCharacter::GetCombatJumpingAnimation()
{
	if (PlayerCombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

		if (ActiveLoadout.MainHandWeapon)
		{
			UAnimSequence* AnimSequence = ActiveLoadout.MainHandWeapon->GetCombatJumpAnimationOverride();
			if (AnimSequence)
			{
				return AnimSequence;
			}
		}
		else if (ActiveLoadout.OffHandWeapon)
		{
			UAnimSequence* AnimSequence = ActiveLoadout.OffHandWeapon->GetCombatJumpAnimationOverride();
			if (AnimSequence)
			{
				return AnimSequence;
			}
		}

		ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
		UAnimSequence** AnimSequencePtr = CombatJumpingAnimationMap.Find(LoadoutType);
		if (AnimSequencePtr)
		{
			return *AnimSequencePtr;
		}
	}

	return DefaultJumpingAnimation;
}

UBlendSpace1D* ADungeonCharacter::GetCombatFallingBlendSpace()
{
	if (PlayerCombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

		if (ActiveLoadout.MainHandWeapon)
		{
			UBlendSpace1D* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatFallingBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}
		else if (ActiveLoadout.OffHandWeapon)
		{
			UBlendSpace1D* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatFallingBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}

		ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
		UBlendSpace1D** BlendSpacePtr = CombatFallingBlendSpaceMap.Find(LoadoutType);
		if (BlendSpacePtr)
		{
			return *BlendSpacePtr;
		}
	}

	return DefaultFallingBlendSpace;
}

UBlendSpace* ADungeonCharacter::GetCombatLandingBlendSpace()
{
	if (PlayerCombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

		if (ActiveLoadout.MainHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatLandingBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}
		else if (ActiveLoadout.OffHandWeapon)
		{
			UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatLandingBlendSpaceOverride();
			if (BlendSpace)
			{
				return BlendSpace;
			}
		}

		ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
		UBlendSpace** BlendSpacePtr = CombatLandingBlendSpaceMap.Find(LoadoutType);
		if (BlendSpacePtr)
		{
			return *BlendSpacePtr;
		}
	}

	return DefaultLandingBlendSpace;
}

float ADungeonCharacter::GetAimYaw()
{
	return AimYaw;
}

float ADungeonCharacter::GetAimPitch()
{
	return AimPitch;
}

void ADungeonCharacter::CalculateAimRotation()
{
	FRotator ControlRotation = GetControlRotation();
	FRotator ActorRotation = GetActorRotation();

	FRotator DeltaRotation = ControlRotation - ActorRotation;
	AimYaw = FMath::ClampAngle(DeltaRotation.Yaw, AimYawClampMin, AimYawClampMax);
	AimPitch = FMath::ClampAngle(DeltaRotation.Pitch, AimPitchClampMin, AimPitchClampMax);

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		FVector Velocity = MovementComp->Velocity;

		if (Velocity.Size() > 0 && !GetIsFreeLooking())
		{
			// No aim offsets when moving unless aiming a bow/spell or free looking
			bIsReorientingBody = false;
			UseControllerDesiredRotation(true);
		}
		else
		{
			if (!bIsReorientingBody)
			{
				if (FMath::Abs(AimYaw) >= AimYawTurnStart && !GetIsFreeLooking())
				{
					bIsReorientingBody = true;
					UseControllerDesiredRotation(true);
				}
				else
				{
					// Allow aim offsets when not moving
					UseControllerDesiredRotation(false);
				}
			}
			else if (bIsReorientingBody && FMath::Abs(AimYaw) <= AimYawTurnStop)
			{
				bIsReorientingBody = false;
			}
		}
	}
}

void ADungeonCharacter::Server_AttachActorToSocket_Implementation(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	Multicast_AttachActorToSocket(Actor, SocketName, RelativePosition, RelativeRotation);
}

bool ADungeonCharacter::Server_AttachActorToSocket_Validate(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	return true;
}

void ADungeonCharacter::Server_DetachActor_Implementation(AActor* Actor)
{
	Multicast_DetachActor(Actor);
}

bool ADungeonCharacter::Server_DetachActor_Validate(AActor* Actor)
{
	return true;
}

UModularCharacterMeshComponent* ADungeonCharacter::GetModularCharacterMeshComponent()
{
	return ModularCharacterMesh;
}

URenderCaptureComponent* ADungeonCharacter::GetRenderCaptureComponent()
{
	return RenderCaptureComponent;
}

ACharacterRenderCapture2D* ADungeonCharacter::GetRenderCaptureActor()
{
	return RenderCaptureComponent->GetRenderCaptureActor();
}

UPlayerCombatComponent* ADungeonCharacter::GetCombatComponent()
{
	return PlayerCombatComponent;
}

FName ADungeonCharacter::GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType)
{
	FName* NamePtr = WeaponSocketMap.Find(WeaponSocketType);
	if (NamePtr)
	{
		return *NamePtr;
	}

	return FName();
}

UInventoryComponent* ADungeonCharacter::GetInventoryComponent()
{
	return InventoryComponent;
}

UEquipmentComponent* ADungeonCharacter::GetEquipmentComponent()
{
	return EquipmentComponent;
}

void ADungeonCharacter::Server_RequestAddItemToInventory_Implementation(AItem* Item)
{
	bool WasItemAdded = InventoryComponent->RequestAddItem(Item);
	if (WasItemAdded)
	{
		Item->Server_Despawn();
	}
	Multicast_AddItemToInventoryResponse(Item, WasItemAdded);
}

bool ADungeonCharacter::Server_RequestAddItemToInventory_Validate(AItem* Item)
{
	return true;
}

void ADungeonCharacter::Server_RequestAddItemToInventoryAtLocation_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	bool WasItemAdded = InventoryComponent->RequestAddItem(Item, OriginSlot);
	if (WasItemAdded)
	{
		Item->Server_Despawn();
	}
	Multicast_AddItemToInventoryResponse(Item, WasItemAdded);
}

bool ADungeonCharacter::Server_RequestAddItemToInventoryAtLocation_Validate(AItem* Item, FInventoryGridPair OriginSlot)
{
	return true;
}

void ADungeonCharacter::Multicast_AddItemToInventoryResponse_Implementation(AItem* Item, bool WasAdded)
{
	if (IsLocallyControlled())
	{

	}
}

void ADungeonCharacter::Server_RequestPickUpItem_Implementation(AItem* Item)
{
	bool WasItemAdded = false;
	bool WasItemEquipped = false;

	// Try equipping the item first, if there is an appropriate open slot
	AEquippable* Equippable = Cast<AEquippable>(Item);
	if (Equippable)
	{
		TArray<EEquipmentSlot> OpenEquipmentSlots = EquipmentComponent->GetOpenSlotsForEquippable(Equippable);

		// If trying to equip a two handed weapon, make sure both main and off hand slots are open
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (Weapon)
		{
			if (Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
			{
				if (EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand))
				{
					OpenEquipmentSlots.Remove(EEquipmentSlot::WeaponLoadoutOneMainHand);
				}
				if (EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand))
				{
					OpenEquipmentSlots.Remove(EEquipmentSlot::WeaponLoadoutTwoMainHand);
				}
			}
		}

		if (OpenEquipmentSlots.Num() > 0)
		{
			EEquipmentSlot SlotToEquipItem = OpenEquipmentSlots[0];

			WasItemEquipped = EquipmentComponent->RequestEquipItem(Equippable, SlotToEquipItem);
		}
	}
	if (WasItemEquipped)
	{
		// Don't "despawn" weapons since they need to be directly attached to the character mesh
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (!Weapon)
		{
			Equippable->Server_Despawn();
		}
	}
	else
	{
		WasItemAdded = InventoryComponent->RequestAddItem(Item);
	}
	if (WasItemAdded)
	{
		Item->Server_Despawn();
	}
	Multicast_PickUpItemResponse(Item, (WasItemAdded || WasItemEquipped));
}

bool ADungeonCharacter::Server_RequestPickUpItem_Validate(AItem* Item)
{
	return true;
}

void ADungeonCharacter::Multicast_PickUpItemResponse_Implementation(AItem* Item, bool WasPickedUp)
{
	if (IsLocallyControlled())
	{
		if (WasPickedUp)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), Item->GetInteractionSound());
		}
	}
}

void ADungeonCharacter::Server_RequestRemoveItemFromInventory_Implementation(AItem* Item)
{
	bool WasItemRemoved = InventoryComponent->RequestRemoveItem(Item);

	Multicast_RemoveItemFromInventoryResponse(Item, WasItemRemoved);
}

bool ADungeonCharacter::Server_RequestRemoveItemFromInventory_Validate(AItem* Item)
{
	return true;
}

void ADungeonCharacter::Multicast_RemoveItemFromInventoryResponse_Implementation(AItem* Item, bool WasRemoved)
{
	if (IsLocallyControlled())
	{

	}
}

void ADungeonCharacter::Server_RequestDropItem_Implementation(AItem* Item, bool CheckInventory /*= true*/)
{
	bool WasItemRemoved = true;
	if (CheckInventory)
	{
		WasItemRemoved = InventoryComponent->RequestRemoveItem(Item);
	}
	if (WasItemRemoved)
	{
		Item->Server_SpawnAtLocation(ItemDropLocation->GetComponentLocation());
	}
	Multicast_DropItemResponse(Item, WasItemRemoved);
}

bool ADungeonCharacter::Server_RequestDropItem_Validate(AItem* Item, bool CheckInventory /*= true*/)
{
	return true;
}

void ADungeonCharacter::Multicast_DropItemResponse_Implementation(AItem* Item, bool WasDropped)
{
	if (IsLocallyControlled())
	{

	}
}

void ADungeonCharacter::Server_RequestEquipItem_Implementation(AEquippable* Equippable, bool TryMoveReplacementToInventory /*= false*/)
{
	TArray<EEquipmentSlot> OpenEquipmentSlots = EquipmentComponent->GetOpenSlotsForEquippable(Equippable);
	TArray<EEquipmentSlot> ValidEquipmentSlots = EquipmentComponent->GetValidSlotsForEquippable(Equippable);
	if (OpenEquipmentSlots.Num() > 0)
	{
		EEquipmentSlot SlotToEquipItem = OpenEquipmentSlots[0];

		Server_RequestEquipItemToSlot(Equippable, SlotToEquipItem, TryMoveReplacementToInventory);
	}
	else if (ValidEquipmentSlots.Num() > 0)
	{
		EEquipmentSlot SlotToEquipItem = ValidEquipmentSlots[0];

		Server_RequestEquipItemToSlot(Equippable, SlotToEquipItem, TryMoveReplacementToInventory);
	}
}

bool ADungeonCharacter::Server_RequestEquipItem_Validate(AEquippable* Equippable, bool TryMoveReplacementToInventory /*= false*/)
{
	return true;
}

void ADungeonCharacter::Server_RequestEquipItemToSlot_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory /*= false*/)
{
	// Unequip the item but don't move it back to the inventory until after the replacement item has been equipped, so we know there will be room in the inventory
	AEquippable* EquipmentInSlot = EquipmentComponent->GetEquipmentInSlot(EquipmentSlot);
	if (EquipmentInSlot)
	{
		Server_RequestUnequipItem(EquipmentInSlot, EquipmentSlot, false);
	}

	// If equipping a two handed weapon, unequip anything in the off hand slot. If equipping an offhand, unequip any equipped two hander.
	AWeapon* WeaponToUnequip = nullptr;
	AWeapon* Weapon = Cast<AWeapon>(Equippable);
	if (Weapon)
	{
		if (Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
		{
			if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutOneMainHand)
			{
				WeaponToUnequip = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand));
				if (WeaponToUnequip)
				{
					Server_RequestUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneOffHand, false);
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoMainHand)
			{
				WeaponToUnequip = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand));
				if (WeaponToUnequip)
				{
					Server_RequestUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoOffHand, false);
				}
			}
		}
		else if (Weapon->GetWeaponHand() == EWeaponHand::OffHand || Weapon->GetWeaponHand() == EWeaponHand::OneHand)
		{
			if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutOneOffHand)
			{
				WeaponToUnequip = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneMainHand));
				if (WeaponToUnequip && WeaponToUnequip->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					Server_RequestUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneMainHand, false);
				}
				else
				{
					WeaponToUnequip = nullptr;
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoOffHand)
			{
				WeaponToUnequip = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoMainHand));
				if (WeaponToUnequip && WeaponToUnequip->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					Server_RequestUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoMainHand, false);
				}
				else
				{
					WeaponToUnequip = nullptr;
				}
			}
		}
	}

	bool WasItemEquipped = EquipmentComponent->RequestEquipItem(Equippable, EquipmentSlot);
	if (WasItemEquipped)
	{
		// Don't "despawn" weapons since they need to be directly attached to the character mesh
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (!Weapon)
		{
			Equippable->Server_Despawn();
		}
	}

	// Now try adding any replaced equipment back to the inventory
	if (EquipmentInSlot && TryMoveReplacementToInventory)
	{
		// Spawn the item in front of the player in case the request to add it back to the inventory fails
		EquipmentInSlot->Server_SpawnAtLocation(ItemDropLocation->GetComponentLocation());
		Server_RequestAddItemToInventory(EquipmentInSlot);
	}

	// Now try adding any unequipped weapon back to the inventory
	if (WeaponToUnequip)
	{
		// Spawn the item in front of the player in case the request to add it back to the inventory fails
		WeaponToUnequip->Server_SpawnAtLocation(ItemDropLocation->GetComponentLocation());
		Server_RequestAddItemToInventory(WeaponToUnequip);
	}

	Multicast_EquipItemResponse(Equippable, WasItemEquipped);
}

bool ADungeonCharacter::Server_RequestEquipItemToSlot_Validate(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory /*= false*/)
{
	return true;
}

void ADungeonCharacter::Multicast_EquipItemResponse_Implementation(AEquippable* Equippable, bool WasEquipped)
{
	if (IsLocallyControlled())
	{

	}
}

void ADungeonCharacter::Server_RequestUnequipItem_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory /*= false*/)
{
	bool WasItemUnequipped = false;

	AEquippable* EquipmentInSlot = EquipmentComponent->GetEquipmentInSlot(EquipmentSlot);
	if (EquipmentInSlot && EquipmentInSlot == Equippable)
	{
		bool WasItemUnequipped = EquipmentComponent->RequestUnequipItem(Equippable, EquipmentSlot);
		if (WasItemUnequipped && TryMoveToInventory)
		{
			bool WasItemMovedToInventory = InventoryComponent->RequestAddItem(Equippable);
			if (WasItemMovedToInventory)
			{
				Equippable->Server_Despawn();
			}
			else
			{
				Equippable->Server_SpawnAtLocation(ItemDropLocation->GetComponentLocation());
			}
		}
		else
		{
			Equippable->Server_Despawn();
		}
	}

	Multicast_UnequipItemResponse(Equippable, WasItemUnequipped);
}

bool ADungeonCharacter::Server_RequestUnequipItem_Validate(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory /*= false*/)
{
	return true;
}

void ADungeonCharacter::Multicast_UnequipItemResponse_Implementation(AEquippable* Equippable, bool WasUnequipped)
{
	if (IsLocallyControlled())
	{

	}
}

void ADungeonCharacter::Multicast_AttachActorToSocket_Implementation(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	AttachmentRules.bWeldSimulatedBodies = true;

	Actor->AttachToComponent(GetMesh(), AttachmentRules ,SocketName);

	Actor->SetActorRelativeLocation(RelativePosition);
	Actor->SetActorRelativeRotation(RelativeRotation);

	RenderCaptureComponent->AttachActorToSocket(Actor, SocketName, RelativePosition, RelativeRotation);
}

void ADungeonCharacter::Multicast_DetachActor_Implementation(AActor* Actor)
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules::KeepRelativeTransform;
	DetachRules.bCallModify = true;
	Actor->DetachFromActor(DetachRules);

	RenderCaptureComponent->DetachActor(Actor);
}