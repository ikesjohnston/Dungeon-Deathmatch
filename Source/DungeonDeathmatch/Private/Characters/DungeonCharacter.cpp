// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <UnrealNetwork.h>
#include <Engine/World.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "DungeonPlayerController.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "DungeonAbilitySystemComponent.h"
#include "DungeonAttributeSet.h"
#include "DungeonGameplayAbility.h"
#include <GameplayEffect.h>

ADungeonCharacter::ADungeonCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

	//StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UDungeonAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UDungeonAttributeSet>(TEXT("AttributeSet"));

	// Movement
	bIsCrouched = false;
	bIsSprinting = false;
	bIsRolling = false;
	bCanRoll = true;

	// TODO: Make these attributes and initialize them on game startup with passive GAs
	MaxCrouchedWalkingSpeed = 200.0f;
	MaxWalkingSpeed = 400.0f;
	MaxSprintingSpeed = 800.0f;
	MaxRollingSpeed = 1200.0f;

	bAbilitiesInitialized = false;
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

bool ADungeonCharacter::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void ADungeonCharacter::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UDungeonGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ADungeonCharacter::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
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

void ADungeonCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ADungeonCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ADungeonCharacter::OnSprintPressed()
{
	AbilitySystemComponent->TryActivateAbilityByClass(StartSprintAbility);
}

void ADungeonCharacter::OnSprintReleased()
{
	AbilitySystemComponent->TryActivateAbilityByClass(StopSprintAbility);
}

void ADungeonCharacter::BeginSprint()
{
	////Server_BeginSprint();
	//if (!bIsRolling)
	//{
	//	bIsSprinting = true;
	//	StatusComponent->SetCanRegenStamina(false);
	//	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	//	if (MovementComp)
	//	{
	//		MovementComp->MaxWalkSpeed = MaxSprintingSpeed;
	//		MovementComp->UnCrouch(true);
	//		MovementComp->bWantsToCrouch = false;
	//		bIsCrouching = false;
	//	}
	//}
}

void ADungeonCharacter::Server_BeginSprint_Implementation()
{
	/*if (!bIsRolling)
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
	}*/
}

bool ADungeonCharacter::Server_BeginSprint_Validate()
{
	return true;
}

void ADungeonCharacter::EndSprint()
{
	//Server_EndSprint();

	/*if (bIsSprinting)
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
	}*/
}

void ADungeonCharacter::Server_EndSprint_Implementation()
{
	/*if (bIsSprinting)
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
	}*/
}

bool ADungeonCharacter::Server_EndSprint_Validate()
{
	return true;
}

void ADungeonCharacter::Jump()
{
	Server_Jump();
}

void ADungeonCharacter::Server_Jump_Implementation()
{
	/*if (!bIsRolling && StatusComponent->SpendStamina(JumpStaminaCost))
	{
		Super::Jump();
	}*/
}

bool ADungeonCharacter::Server_Jump_Validate()
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
	//if (bCanRoll && StatusComponent->SpendStamina(RollStaminaCost))
	//{
	//	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	//	if (MovementComp && !MovementComp->IsFalling())
	//	{
	//		bIsRolling = true;
	//		bCanRoll = false;
	//		Crouch();
	//		MovementComp->bWantsToCrouch = true;
	//		MovementComp->MaxWalkSpeedCrouched = MaxRollingSpeed;

	//		// Immediately roll at max speed in the player's current movement direction,
	//		// or roll forward if the player is standing still.
	//		FVector VelocityNormalized = MovementComp->Velocity;
	//		VelocityNormalized.Normalize();
	//		if (VelocityNormalized.Size() == 0)
	//			VelocityNormalized = GetActorForwardVector();
	//		MovementComp->Velocity = VelocityNormalized * MaxRollingSpeed;
	//		MovementComp->BrakingDecelerationWalking = 0;
	//		MovementComp->GroundFriction = 0;

	//		// Cancel any ongoing attacks, shouldn't be able to do damage during a roll
	//		CancelAttack();
	//	}
	//}
}

bool ADungeonCharacter::Server_BeginRoll_Validate()
{
	return true;
}

void ADungeonCharacter::EndRoll()
{
	Server_EndRoll();
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

void ADungeonCharacter::SetCanRoll(bool CanRoll)
{
	bCanRoll = CanRoll;
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
	/*ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetController());
	if (PlayerController)
	{
		IFunctionable* CurrentFocus = Cast<IFunctionable>(PlayerController->GetCurrentFocus());
		if (CurrentFocus)
		{
			CurrentFocus->NativeOnStartPrimaryFunction(this);
		}
	}*/
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
	//if (CanAttack())
	//{
	//	AWeapon* Weapon = Cast<AWeapon>(EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::MainHand));
	//	if (Weapon)
	//	{
	//		UWeaponData* WeaponData = Weapon->GetWeaponData();
	//		if (WeaponData)
	//		{
	//			//UAnimMontage* AttackMontage = WeaponData->GetAttackMontage()
	//			/*float ComboOpeningTime = WeaponData->GetComboAttackOpeningTime();
	//			GetWorldTimerManager().ClearTimer(ComboTimer);
	//			GetWorldTimerManager().SetTimer(ComboTimer, this, &ADungeonCharacter::SetWeaponReady, ComboOpeningTime, false);

	//			switch (ComboState)
	//			{
	//			case EComboState::ComboReset:
	//				ComboState = EComboState::Attack1;
	//				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack one."));
	//				break;
	//			case EComboState::Attack1:
	//				if (CombatState == ECombatState::AttackingComboReady)
	//				{
	//					ComboState = EComboState::Attack2;
	//					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack two."));
	//					CombatState = ECombatState::Attacking;
	//				}
	//				break;
	//			case EComboState::Attack2:
	//				if (CombatState == ECombatState::AttackingComboReady)
	//				{
	//					ComboState = EComboState::Attack3;
	//					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Performing attack three."));
	//					CombatState = ECombatState::Attacking;
	//				}
	//				break;
	//			case EComboState::Attack3:
	//				break;
	//			default:
	//				break;
	//			}*/
	//			
	//			//CombatState = ECombatState::Attacking;
	//		}
	//	}
	//	else if (AnimationProfile)
	//	{

	//		UAnimMontage* AttackMontage = AnimationProfile->GetAttackOneMontage();
	//		if (AttackMontage)
	//		{
	//			Multicast_PlayAnimMontage(AttackMontage);
	//		}
	//	}
	//}
}

bool ADungeonCharacter::Server_Attack_Validate()
{
	return true;
}

UAnimationProfile* ADungeonCharacter::GetAnimationProfile()
{
	return AnimationProfile;
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

void ADungeonCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorPawn, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorPawn, DamageCauser);
}

void ADungeonCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
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
	DetachFromControllerPendingDestroy();
	OnDeath();
	//GetMesh()->SetSimulatePhysics(true);
}