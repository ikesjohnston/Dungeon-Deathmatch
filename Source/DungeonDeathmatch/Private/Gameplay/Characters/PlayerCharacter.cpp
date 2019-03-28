// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Interactables/InteractableInterface.h"
#include "DungeonAbilitySystemComponent.h"
#include "DungeonGameplayAbility.h"
#include "DungeonPlayerController.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "PlayerCombatComponent.h"
#include "RenderCaptureComponent.h"
#include "CharacterRenderCapture2D.h"
#include "ModularHumanoidMeshComponent.h"
#include "CharacterAnimationComponent.h"

#include <WidgetComponent.h>
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include <Private/KismetTraceUtils.h>
#include <AbilitySystemComponent.h>
#include <UnrealNetwork.h>

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	VitalsPlateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("VitalsPlateWidget"));
	VitalsPlateWidget->SetupAttachment(GetCapsuleComponent());
	VitalsPlateWidget->SetWidgetSpace(EWidgetSpace::Screen);
	VitalsPlateWidget->bOwnerNoSee = true;
	VitalsPlateWidget->bOnlyOwnerSee = false;

	ModularHumanoidMesh = CreateDefaultSubobject<UModularHumanoidMeshComponent>(TEXT("ModularHumanoidMesh"));
	RenderCaptureComponent = CreateDefaultSubobject<URenderCaptureComponent>(TEXT("RenderCaptureComponent"));
	PlayerCombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));
	AnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("AnimationComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bIsMovementInputEnabled = true;
	bIsCameraInputEnabled = true;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, bIsMovementInputEnabled);
}

void APlayerCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	ModularHumanoidMesh->Initialize();
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
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

	if (IsLocallyControlled())
	{
		// Only show health plates on enemy characters
		VitalsPlateWidget->SetVisibility(false);
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void APlayerCharacter::InitializeAbilities()
{
	Super::InitializeAbilities();

	if (AbilitySystemComponent && HasAuthority())
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
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement Inputs
	PlayerInputComponent->BindAxis("Move Forward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::LookRight);
	PlayerInputComponent->BindAction("Free Look", IE_Pressed, this, &APlayerCharacter::OnFreeLookKeyPressed);
	PlayerInputComponent->BindAction("Free Look", IE_Released, this, &APlayerCharacter::OnFreeLookKeyReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::OnSprintKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::OnSprintKeyReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::OnCrouchKeyPressed);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnJumpKeyPressed);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::OnRollKeyPressed);

	// Action Inputs
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::OnInteractKeyPressed);
	PlayerInputComponent->BindAction("Switch Loadout", IE_Pressed, this, &APlayerCharacter::OnLoadoutSwitchKeyPressed);
	PlayerInputComponent->BindAction("Sheathe Weapon", IE_Pressed, this, &APlayerCharacter::OnSheatheKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Attack", IE_Pressed, this, &APlayerCharacter::OnMainHandAttackKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Attack", IE_Released, this, &APlayerCharacter::OnMainHandAttackKeyReleased);
	PlayerInputComponent->BindAction("Main Hand Alt. Attack", IE_Pressed, this, &APlayerCharacter::OnMainHandAltAttackKeyPressed);
	PlayerInputComponent->BindAction("Main Hand Alt. Attack", IE_Released, this, &APlayerCharacter::OnMainHandAltAttackKeyReleased);
	PlayerInputComponent->BindAction("Off Hand Attack", IE_Pressed, this, &APlayerCharacter::OnOffHandAttackKeyPressed);
	PlayerInputComponent->BindAction("Off Hand Attack", IE_Released, this, &APlayerCharacter::OnOffHandAttackKeyReleased);
	PlayerInputComponent->BindAction("Off Hand Alt. Attack", IE_Pressed, this, &APlayerCharacter::OnOffHandAltAttackKeyPressed);
	PlayerInputComponent->BindAction("Off Hand Alt. Attack", IE_Released, this, &APlayerCharacter::OnOffHandAltAttackKeyReleased);

	// Menu Inputs
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &APlayerCharacter::OnInventoryKeyPressed);
	PlayerInputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &APlayerCharacter::OnEscapeKeyPressed);

	//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerCharacter::NativeOnDeath()
{
	Super::NativeOnDeath();

	// Disable health plates on dead players
	VitalsPlateWidget->Deactivate();
	VitalsPlateWidget->SetVisibility(false);
}

ACharacterRenderCapture2D* APlayerCharacter::GetRenderCaptureActor()
{
	return RenderCaptureComponent->GetRenderCaptureActor();
}

void APlayerCharacter::MoveForward(float Value)
{
	if (bIsMovementInputEnabled)
	{
		AddMovementInput(GetActorForwardVector() * Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (bIsMovementInputEnabled)
	{
		AddMovementInput(GetActorRightVector() * Value);
	}
}

void APlayerCharacter::LookRight(float YawInput)
{
	if (bIsCameraInputEnabled)
	{
		AddControllerYawInput(YawInput);
	}
}

void APlayerCharacter::LookUp(float PitchInput)
{
	if (bIsCameraInputEnabled)
	{
		AddControllerPitchInput(PitchInput);
	}
}

void APlayerCharacter::Jump()
{
	if (bIsMovementInputEnabled)
	{
		Super::Jump();

		AnimationComponent->SetIsJumping(true);
	}
}

void APlayerCharacter::OnJumpKeyPressed()
{
	if (JumpAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(JumpAbility);
	}
}

void APlayerCharacter::OnSprintKeyPressed()
{
	if (StartSprintAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StartSprintAbility);
	}
}

void APlayerCharacter::OnSprintKeyReleased()
{
	if (StopSprintAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StopSprintAbility);
	}
}

void APlayerCharacter::OnFreeLookKeyPressed()
{
	if (StartFreeLookAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StartFreeLookAbility);
	}
}

void APlayerCharacter::OnFreeLookKeyReleased()
{
	if (StopFreeLookAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(StopFreeLookAbility);
	}
}

void APlayerCharacter::OnCrouchKeyPressed()
{
	if (CrouchAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(CrouchAbility);
	}
}

void APlayerCharacter::OnRollKeyPressed()
{
	if (RollAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(RollAbility);
	}
}

void APlayerCharacter::OnInteractKeyPressed()
{
	Interact();
}

void APlayerCharacter::OnLoadoutSwitchKeyPressed()
{
	PlayerCombatComponent->SwitchLoadout();
}

void APlayerCharacter::OnSheatheKeyPressed()
{
	PlayerCombatComponent->ToggleWeaponSheathe();
}

void APlayerCharacter::OnMainHandAttackKeyPressed()
{
	PlayerCombatComponent->PerformMainHandAttack();
}

void APlayerCharacter::OnMainHandAttackKeyReleased()
{

}

void APlayerCharacter::OnMainHandAltAttackKeyPressed()
{
	PlayerCombatComponent->PerformMainHandAltAttack();
}

void APlayerCharacter::OnMainHandAltAttackKeyReleased()
{

}

void APlayerCharacter::OnOffHandAttackKeyPressed()
{
	PlayerCombatComponent->PerformOffHandAttack();
}

void APlayerCharacter::OnOffHandAttackKeyReleased()
{

}

void APlayerCharacter::OnOffHandAltAttackKeyPressed()
{
	PlayerCombatComponent->PerformOffHandAltAttack();
}

void APlayerCharacter::OnOffHandAltAttackKeyReleased()
{

}

void APlayerCharacter::OnInventoryKeyPressed()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnInventoryKeyPressed();
	}
}

void APlayerCharacter::OnEscapeKeyPressed()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->OnEscapeKeyPressed();
	}
}

void APlayerCharacter::Interact()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		AActor* FocusedInteractable = PlayerController->GetFocusedInteractable();
		if (FocusedInteractable)
		{
			IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(FocusedInteractable);
			if (InteractableInterface)
			{
				InteractableInterface->Execute_OnInteract(FocusedInteractable, this);
			}
		}
	}
}