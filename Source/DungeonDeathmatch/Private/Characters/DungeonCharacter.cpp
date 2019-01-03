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

	//GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// --------------- Begin Mesh Segment Setup ---------------
	MeshComponentHelm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHelm"));
	MeshComponentHelm->SetupAttachment(GetMesh());
	MeshComponentHelm->SetMasterPoseComponent(GetMesh());

	MeshComponentHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHead"));
	MeshComponentHead->SetupAttachment(GetMesh());
	MeshComponentHead->SetMasterPoseComponent(GetMesh());

	MeshComponentShoulderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderLeft"));
	MeshComponentShoulderLeft->SetupAttachment(GetMesh());
	MeshComponentShoulderLeft->SetMasterPoseComponent(GetMesh());

	MeshComponentShoulderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderRight"));
	MeshComponentShoulderRight->SetupAttachment(GetMesh());
	MeshComponentShoulderRight->SetMasterPoseComponent(GetMesh());

	MeshComponentTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentTorso"));
	MeshComponentTorso->SetupAttachment(GetMesh());
	MeshComponentTorso->SetMasterPoseComponent(GetMesh());

	MeshComponentChestArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentChestArmor"));
	MeshComponentChestArmor->SetupAttachment(GetMesh());
	MeshComponentChestArmor->SetMasterPoseComponent(GetMesh());

	MeshComponentHandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandLeft"));
	MeshComponentHandLeft->SetupAttachment(GetMesh());
	MeshComponentHandLeft->SetMasterPoseComponent(GetMesh());

	MeshComponentHandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandRight"));
	MeshComponentHandRight->SetupAttachment(GetMesh());
	MeshComponentHandRight->SetMasterPoseComponent(GetMesh());

	MeshComponentBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentBelt"));
	MeshComponentBelt->SetupAttachment(GetMesh());
	MeshComponentBelt->SetMasterPoseComponent(GetMesh());

	MeshComponentLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegs"));
	MeshComponentLegs->SetupAttachment(GetMesh());
	MeshComponentLegs->SetMasterPoseComponent(GetMesh());

	MeshComponentLegArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegArmor"));
	MeshComponentLegArmor->SetupAttachment(GetMesh());
	MeshComponentLegArmor->SetMasterPoseComponent(GetMesh());

	MeshComponentFootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootLeft"));
	MeshComponentFootLeft->SetupAttachment(GetMesh());
	MeshComponentFootLeft->SetMasterPoseComponent(GetMesh());

	MeshComponentFootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootRight"));
	MeshComponentFootRight->SetupAttachment(GetMesh());
	MeshComponentFootRight->SetMasterPoseComponent(GetMesh());
	// --------------- End Mesh Segment Setup -----------------

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

	//DungeonCharMovementComponent = CreateDefaultSubobject<UDungeonCharMovementComponent>(TEXT("DungeonCharMovementComponent"));
	
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

	BaseStandingMovementSpeed = 400.0f;
	GetCharacterMovement()->MaxWalkSpeed = BaseStandingMovementSpeed;
	BaseCrouchedMovementSpeed = 200.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = BaseCrouchedMovementSpeed;

	bIsMovementInputEnabled = true;
	bCanLook = true;

	AimYawTurnStart = 60;
	AimYawTurnStop = 5;
	AimYawClampMin = -90;
	AimYawClampMax = 90;
	AimPitchClampMin = -90;
	AimPitchClampMax = 90;

}

void ADungeonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonCharacter, CurrentMeleeComboState);
	DOREPLIFETIME(ADungeonCharacter, bIsMeleeComboReady);

	DOREPLIFETIME(ADungeonCharacter, AimYaw);
	DOREPLIFETIME(ADungeonCharacter, AimPitch);
	DOREPLIFETIME(ADungeonCharacter, bIsReorientingBody);
	DOREPLIFETIME(ADungeonCharacter, bIsMovementInputEnabled);
	DOREPLIFETIME(ADungeonCharacter, bIsJumping);
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
	FVector Velocity = GetMovementVelocity();
	FRotator ActorRotation = GetActorRotation();
	FVector LocalVelocity = ActorRotation.UnrotateVector(Velocity);
	float MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));

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

bool ADungeonCharacter::GetIsMovementInputEnabled()
{
	return bIsMovementInputEnabled;
}

void ADungeonCharacter::SetIsMovementInputEnabled(bool IsMovementInputEnabled)
{
	bIsMovementInputEnabled = IsMovementInputEnabled;
}

bool ADungeonCharacter::GetIsJumping()
{
	return bIsJumping;
}

void ADungeonCharacter::SetIsJumping(bool IsJumping)
{
	bIsJumping = IsJumping;
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

void ADungeonCharacter::Jump()
{
	if (bIsMovementInputEnabled)
	{
		Super::Jump();

		SetIsJumping(true);
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
		AActor* FocusedInteractable = PlayerController->GetFocusedInteractable();
		if (FocusedInteractable)
		{
			IInteractable* InteractableInterface = Cast<IInteractable>(FocusedInteractable);
			if (InteractableInterface)
			{
				InteractableInterface->Execute_OnInteract(FocusedInteractable, this);
			}
			else
			{
				AItem* Item = Cast<AItem>(FocusedInteractable);
				if (Item)
				{
					Server_TryPickUpItem(Item);
				}
				else
				{
					//FocusedInteractable->Server_OnInteract(this);
				}
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

bool ADungeonCharacter::GetIsReorientingBody()
{
	return bIsReorientingBody;
}

float ADungeonCharacter::GetAimYaw()
{
	return AimYaw;
}

float ADungeonCharacter::GetAimPitch()
{
	return AimPitch;
}

void ADungeonCharacter::UpdateMeshSegments(TMap<EMeshSegment, USkeletalMesh*> MeshMap)
{
	for (TPair<EMeshSegment, USkeletalMesh*> MeshPair : MeshMap)
	{
		switch (MeshPair.Key)
		{
		case EMeshSegment::Head:
			MeshComponentHelm->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::LeftShoulder:
			MeshComponentShoulderLeft->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::RightShoulder:
			MeshComponentShoulderRight->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::Chest:
			MeshComponentTorso->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::Waist:
			MeshComponentBelt->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::Legs:
			MeshComponentLegArmor->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::LeftFoot:
			MeshComponentFootLeft->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::RightFoot:
			MeshComponentFootRight->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::LeftHand:
			MeshComponentHandLeft->SetSkeletalMesh(MeshPair.Value);
			break;
		case EMeshSegment::RightHand:
			MeshComponentHandRight->SetSkeletalMesh(MeshPair.Value);
			break;
		default:
			break;
		}
	}
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

		if (Velocity.Size() > 0)
		{
			// No aim offsets when moving unless aiming a bow/spell
			bIsReorientingBody = false;
			UseControllerDesiredRotation(true);
		}
		else
		{
			if (!bIsReorientingBody)
			{
				if (FMath::Abs(AimYaw) >= AimYawTurnStart)
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
