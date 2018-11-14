// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <UnrealNetwork.h>
#include <Engine/World.h>
#include "Weapon.h"
#include "StatusComponent.h"
#include <GameFramework/Character.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "DungeonPlayerController.h"
#include "Functionable.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "Weapons/WeaponData.h"
#include "DungeonAbilitySystemComponent.h"

// Sets default values
ADungeonCharacter::ADungeonCharacter()
{
	bReplicates = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	//GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	FistColliderLeft = CreateDefaultSubobject<USphereComponent>(TEXT("FistColliderLeft"));
	FistColliderLeft->SetupAttachment(GetMesh(), "HandLeft");

	FistColliderRight = CreateDefaultSubobject<USphereComponent>(TEXT("FistColliderRight"));
	FistColliderRight->SetupAttachment(GetMesh(), "HandRight");

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UDungeonAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Movement
	JumpStaminaCost = 10;
	bIsCrouched = false;
	bIsSprinting = false;
	SprintStaminaCost = 10;
	bIsRolling = false;
	bCanRoll = true;
	RollStaminaCost = 30;
	MaxCrouchedWalkingSpeed = 200.0f;
	MaxWalkingSpeed = 400.0f;
	MaxSprintingSpeed = 800.0f;
	MaxRollingSpeed = 1200.0f;
}

void ADungeonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonCharacter, bIsCrouching);
	DOREPLIFETIME(ADungeonCharacter, bIsSprinting);
	DOREPLIFETIME(ADungeonCharacter, bIsRolling);
	DOREPLIFETIME(ADungeonCharacter, bCanRoll);
}

// Called when the game starts or when spawned
void ADungeonCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->MaxWalkSpeed = MaxWalkingSpeed;
		MovementComp->MaxWalkSpeedCrouched = MaxCrouchedWalkingSpeed;
		DefaultWalkingDeceleration = MovementComp->BrakingDecelerationWalking;
		DefaultGroundFriction = MovementComp->GroundFriction;
	}

	if(AbilitySystemComponent)
	{
		if (HasAuthority())
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StartSprintAbility.GetDefaultObject()), 1, 0));
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(StopSprintAbility.GetDefaultObject()), 1, 0));
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ADungeonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

// Called every frame
void ADungeonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Server_Tick(DeltaTime);
}

// Called to bind functionality to input
void ADungeonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement Inputs
	PlayerInputComponent->BindAxis("MoveForward", this, &ADungeonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADungeonCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ADungeonCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ADungeonCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADungeonCharacter::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADungeonCharacter::OnSprintReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADungeonCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ADungeonCharacter::BeginRoll);

	// Combat Inputs
	PlayerInputComponent->BindAction("Sheathe", IE_Pressed, this, &ADungeonCharacter::OnSheathePressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ADungeonCharacter::OnAttackPressed);

	// Action Inputs
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ADungeonCharacter::OnUsePressed);

	//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));

}

UAbilitySystemComponent* ADungeonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADungeonCharacter::Server_Tick_Implementation(float DeltaTime)
{
	if (bIsSprinting)
	{
		if (!StatusComponent->SpendStamina(SprintStaminaCost * DeltaTime))
		{
			EndSprint();
		}
	}
}

bool ADungeonCharacter::Server_Tick_Validate(float DeltaTime)
{
	return true;
}

void ADungeonCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ADungeonCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ADungeonCharacter::Jump()
{
	Server_Jump();
}

void ADungeonCharacter::OnSprintPressed()
{
	AbilitySystemComponent->TryActivateAbilityByClass(StartSprintAbility);
}

void ADungeonCharacter::OnSprintReleased()
{
	AbilitySystemComponent->TryActivateAbilityByClass(StopSprintAbility);
}

void ADungeonCharacter::Server_Jump_Implementation()
{
	if (!bIsRolling && StatusComponent->SpendStamina(JumpStaminaCost))
	{
		Super::Jump();
	}
}

bool ADungeonCharacter::Server_Jump_Validate()
{
	return true;
}

void ADungeonCharacter::BeginSprint()
{
	//Server_BeginSprint();
	if (!bIsRolling)
	{
		bIsSprinting = true;
		StatusComponent->SetCanRegenStamina(false);
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed = MaxSprintingSpeed;
			MovementComp->UnCrouch(true);
			MovementComp->bWantsToCrouch = false;
			bIsCrouching = false;
		}
	}
}

void ADungeonCharacter::Server_BeginSprint_Implementation()
{
	if (!bIsRolling)
	{
		bIsSprinting = true;
		StatusComponent->SetCanRegenStamina(false);
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed = MaxSprintingSpeed;
			MovementComp->UnCrouch(true);
			MovementComp->bWantsToCrouch = false;
			bIsCrouching = false;
		}
	}
}

bool ADungeonCharacter::Server_BeginSprint_Validate()
{
	return true;
}

void ADungeonCharacter::EndSprint()
{
	//Server_EndSprint();

	if (bIsSprinting)
	{
		bIsSprinting = false;
		StatusComponent->SetCanRegenStamina(true);
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed = MaxWalkingSpeed;
			if (!bIsCrouched && !bIsRolling)
			{
				MovementComp->UnCrouch(true);
				MovementComp->bWantsToCrouch = false;
				bIsCrouching = false;
			}
		}
	}
}

void ADungeonCharacter::Server_EndSprint_Implementation()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		StatusComponent->SetCanRegenStamina(true);
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed = MaxWalkingSpeed;
			if (!bIsCrouched && !bIsRolling)
			{
				MovementComp->UnCrouch(true);
				MovementComp->bWantsToCrouch = false;
				bIsCrouching = false;
			}
		}
	}
}

bool ADungeonCharacter::Server_EndSprint_Validate()
{
	return true;
}

void ADungeonCharacter::ToggleCrouch()
{
	Server_ToggleCrouch();
}

void ADungeonCharacter::Server_ToggleCrouch_Implementation()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		if (!bIsCrouching)
		{
			Crouch();
			bIsCrouching = true;
			MovementComp->bWantsToCrouch = true;
		}
		else if (bIsCrouching && bIsRolling)
		{
			bIsCrouching = false;
		}
		else if (bIsCrouching && !bIsRolling)
		{
			UnCrouch();
			MovementComp->bWantsToCrouch = false;
			bIsCrouching = false;
		}

		if (bIsSprinting)
		{
			MovementComp->MaxWalkSpeed = MaxSprintingSpeed;
		}
		else
		{
			MovementComp->MaxWalkSpeed = MaxWalkingSpeed;
		}
	}
}

bool ADungeonCharacter::Server_ToggleCrouch_Validate()
{
	return true;
}

void ADungeonCharacter::BeginRoll()
{
	Server_BeginRoll();
}

void ADungeonCharacter::Server_BeginRoll_Implementation()
{
	if (bCanRoll && StatusComponent->SpendStamina(RollStaminaCost))
	{
		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		if (MovementComp && !MovementComp->IsFalling())
		{
			bIsRolling = true;
			bCanRoll = false;
			Crouch();
			MovementComp->bWantsToCrouch = true;
			MovementComp->MaxWalkSpeedCrouched = MaxRollingSpeed;

			// Immediately roll at max speed in the player's current movement direction,
			// or roll forward if the player is standing still.
			FVector VelocityNormalized = MovementComp->Velocity;
			VelocityNormalized.Normalize();
			if (VelocityNormalized.Size() == 0)
				VelocityNormalized = GetActorForwardVector();
			MovementComp->Velocity = VelocityNormalized * MaxRollingSpeed;
			MovementComp->BrakingDecelerationWalking = 0;
			MovementComp->GroundFriction = 0;

			// Cancel any ongoing attacks, shouldn't be able to do damage during a roll
			CancelAttack();
		}
	}
}

bool ADungeonCharacter::Server_BeginRoll_Validate()
{
	return true;
}

void ADungeonCharacter::EndRoll()
{
	Server_EndRoll();
}

void ADungeonCharacter::SetCanRoll(bool CanRoll)
{
	bCanRoll = CanRoll;
}

void ADungeonCharacter::Server_EndRoll_Implementation()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		bIsRolling = false;
		bCanRoll = true;
		MovementComp->BrakingDecelerationWalking = DefaultWalkingDeceleration;
		MovementComp->GroundFriction = DefaultGroundFriction;
		MovementComp->MaxWalkSpeedCrouched = MaxCrouchedWalkingSpeed;
		if (!bIsCrouching)
		{
			UnCrouch();
			MovementComp->bWantsToCrouch = false;
		}
	}
}

bool ADungeonCharacter::Server_EndRoll_Validate()
{
	return true;
}

void ADungeonCharacter::OnUsePressed()
{
	Server_Use();
}

void ADungeonCharacter::Use()
{
	Server_Use();
}

void ADungeonCharacter::Server_Use_Implementation()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		IFunctionable* CurrentFocus = Cast<IFunctionable>(PlayerController->GetCurrentFocus());
		if (CurrentFocus)
		{
			CurrentFocus->NativeOnStartPrimaryFunction(this);
		}
	}
}

bool ADungeonCharacter::Server_Use_Validate()
{
	return true;
}

void ADungeonCharacter::OnSheathePressed()
{
	/*if (CombatState == ECombatState::WeaponSheathed)
		UnsheatheWeapon();
	else if (CombatState == ECombatState::WeaponReady)
		SheatheWeapon();*/
}

void ADungeonCharacter::SheatheWeapon()
{
	Server_SheatheWeapon();
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

void ADungeonCharacter::UnsheatheWeapon()
{
	Server_UnsheatheWeapon();
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

void ADungeonCharacter::OnAttackPressed()
{
	//if (CombatState == ECombatState::WeaponReady)// || CombatState == ECombatState::AttackingComboReady)
		Attack();
}

bool ADungeonCharacter::CanAttack()
{
	if (bIsRolling)
		return false;

	return true;
}

void ADungeonCharacter::Attack()
{
	Server_Attack();
}

void ADungeonCharacter::CancelAttack()
{
	Server_CancelAttack();
}

void ADungeonCharacter::Server_CancelAttack_Implementation()
{
	/*Multicast_StopAllAnimMontages();
	if (CombatState == ECombatState::Attacking)
	{
		CombatState = ECombatState::WeaponReady;
	}*/
}

bool ADungeonCharacter::Server_CancelAttack_Validate()
{
	return true;
}

void ADungeonCharacter::Server_Attack_Implementation()
{
	if (CanAttack())
	{
		AWeapon* Weapon = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::MainHand));
		if (Weapon)
		{
			UWeaponData* WeaponData = Weapon->GetWeaponData();
			if (WeaponData)
			{
				//UAnimMontage* AttackMontage = WeaponData->GetAttackMontage()
				/*float ComboOpeningTime = WeaponData->GetComboAttackOpeningTime();
				GetWorldTimerManager().ClearTimer(ComboTimer);
				GetWorldTimerManager().SetTimer(ComboTimer, this, &ADungeonCharacter::SetWeaponReady, ComboOpeningTime, false);

				switch (ComboState)
				{
				case EComboState::ComboReset:
					ComboState = EComboState::Attack1;
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack one."));
					break;
				case EComboState::Attack1:
					if (CombatState == ECombatState::AttackingComboReady)
					{
						ComboState = EComboState::Attack2;
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack two."));
						CombatState = ECombatState::Attacking;
					}
					break;
				case EComboState::Attack2:
					if (CombatState == ECombatState::AttackingComboReady)
					{
						ComboState = EComboState::Attack3;
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack three."));
						CombatState = ECombatState::Attacking;
					}
					break;
				case EComboState::Attack3:
					break;
				default:
					break;
				}*/
				
				//CombatState = ECombatState::Attacking;
			}
		}
		else if (AnimationProfile)
		{

			UAnimMontage* AttackMontage = AnimationProfile->GetAttackOneMontage();
			if (AttackMontage)
			{
				Multicast_PlayAnimMontage(AttackMontage);
			}
		}
	}
}

bool ADungeonCharacter::Server_Attack_Validate()
{
	return true;
}

UAnimationProfile* ADungeonCharacter::GetAnimationProfile()
{
	return AnimationProfile;
}

//void ADungeonCharacter::SetComboReady()
//{
//	GetWorldTimerManager().ClearTimer(ComboTimer);
//	AWeapon* Weapon = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::MainHand));
//	if (Weapon)
//	{
//		UWeaponData* WeaponData = Weapon->GetWeaponData();
//		if (WeaponData)
//		{
//			float ComboOpeningTime = WeaponData->GetComboAttackOpeningTime();
//
//			CombatState = ECombatState::AttackingComboReady;
//			GetWorldTimerManager().SetTimer(ComboTimer, this, &ADungeonCharacter::SetWeaponReady, ComboOpeningTime, false);
//			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Ready to continue combo..."));
//		}
//	}
//}

void ADungeonCharacter::OnHealthChanged(UStatusComponent* OwningStatusComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && StatusComponent->IsAlive())
	{
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);

		Multicast_OnDeath();
	}
}

void ADungeonCharacter::Multicast_StopAllAnimMontages_Implementation()
{
	USkeletalMeshComponent* CharacterMeshComp = GetMesh();
	if (CharacterMeshComp && CharacterMeshComp->AnimScriptInstance)
	{
		CharacterMeshComp->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void ADungeonCharacter::Multicast_PlayAnimMontage_Implementation(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None)
{
	PlayAnimMontage(AnimMontage);
}

void ADungeonCharacter::Multicast_OnDeath_Implementation()
{
	DetachFromControllerPendingDestroy();
	OnDeath();
	//GetMesh()->SetSimulatePhysics(true);
}