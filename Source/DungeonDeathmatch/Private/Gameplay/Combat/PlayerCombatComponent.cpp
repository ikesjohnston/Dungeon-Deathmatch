// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCombatComponent.h"
#include "PlayerCharacter.h"
#include "EquipmentComponent.h"
#include "EquipmentGlobals.h"
#include "Weapon.h"

#include <AbilitySystemComponent.h>

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;
	
	bIsMeleeComboReady = true;
	CombatState = ECombatState::Sheathed;
}

void UPlayerCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerCombatComponent, CombatState);
	DOREPLIFETIME(UPlayerCombatComponent, ActiveMeleeComboType);
	DOREPLIFETIME(UPlayerCombatComponent, ActiveMeleeComboCount);
	DOREPLIFETIME(UPlayerCombatComponent, bIsMeleeComboReady);
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = Cast<UAbilitySystemComponent>(GetOwner()->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCombatComponent::BeginPlay - Owner does not have an AbilitySystemComponent."))
	}

	EquipmentComponent = Cast<UEquipmentComponent>(GetOwner()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCombatComponent::BeginPlay - Owner does not have an EquipmentComponent."))
	}

	if (GetOwner()->HasAuthority())
	{
		if (SheatheWeaponsAbility)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(SheatheWeaponsAbility.GetDefaultObject()), 1, 0));
		}
		if (UnsheatheWeaponsAbility)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(UnsheatheWeaponsAbility.GetDefaultObject()), 1, 0));
		}
		if (SwitchWeaponLoadoutAbility)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Cast<UGameplayAbility>(SwitchWeaponLoadoutAbility.GetDefaultObject()), 1, 0));
		}
	}
}

void UPlayerCombatComponent::ToggleActiveLoadout()
{
	if (!EquipmentComponent) return;
	EquipmentComponent->ServerToggleActiveLoadout();
}

void UPlayerCombatComponent::SwitchLoadout()
{
	if (!AbilitySystemComponent) return;
	if (SwitchWeaponLoadoutAbility)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(SwitchWeaponLoadoutAbility);
	}
}

void UPlayerCombatComponent::ToggleWeaponSheathe()
{
	if (!AbilitySystemComponent) return;
	if (CombatState == ECombatState::Sheathed)
	{
		if (UnsheatheWeaponsAbility)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(UnsheatheWeaponsAbility);
		}
	}
	else if (CombatState == ECombatState::ReadyToUse)
	{
		if (SheatheWeaponsAbility)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(SheatheWeaponsAbility);
		}
	}
}

void UPlayerCombatComponent::ServerSetCombatState_Implementation(ECombatState NewCombatSate)
{
	CombatState = NewCombatSate;

	switch (CombatState)
	{
	default:
		break;
	case ECombatState::Sheathed:
		break;
	case ECombatState::Unsheathing:
		break;
	case ECombatState::Sheathing:
		break;
	case ECombatState::ReadyToUse:
		break;
	case ECombatState::AttackInProgress:
		break;
	}
}

bool UPlayerCombatComponent::ServerSetCombatState_Validate(ECombatState NewCombatSate)
{
	return true;
}

void UPlayerCombatComponent::ServerSetActiveMeleeComboType_Implementation(EMeleeComboType ComboType)
{
	MulticastSetActiveMeleeComboType(ComboType);
}

bool UPlayerCombatComponent::ServerSetActiveMeleeComboType_Validate(EMeleeComboType ComboType)
{
	return true;
}

void UPlayerCombatComponent::MulticastSetActiveMeleeComboType_Implementation(EMeleeComboType ComboType)
{
	ActiveMeleeComboType = ComboType;
}

void UPlayerCombatComponent::ServerClearMeleeComboCounter_Implementation()
{
	MulticastClearMeleeComboCounter();
}

bool UPlayerCombatComponent::ServerClearMeleeComboCounter_Validate()
{
	return true;
}

void UPlayerCombatComponent::MulticastClearMeleeComboCounter_Implementation()
{
	ActiveMeleeComboCount = 0;
}

void UPlayerCombatComponent::ServerIncrementMeleeComboCounter_Implementation()
{
	MulticastIncrementMeleeComboCounter();
}

bool UPlayerCombatComponent::ServerIncrementMeleeComboCounter_Validate()
{
	return true;
}

void UPlayerCombatComponent::MulticastIncrementMeleeComboCounter_Implementation()
{
	ActiveMeleeComboCount++;
}

void UPlayerCombatComponent::PerformMainHandAttack()
{
	TryPerformAttack(EMeleeComboType::MainHand);
}

void UPlayerCombatComponent::PerformMainHandAltAttack()
{
	TryPerformAttack(EMeleeComboType::MainHandAlt);
}

void UPlayerCombatComponent::PerformOffHandAttack()
{
	TryPerformAttack(EMeleeComboType::OffHand);
}

void UPlayerCombatComponent::PerformOffHandAltAttack()
{
	TryPerformAttack(EMeleeComboType::OffHandAlt);
}

void UPlayerCombatComponent::TryPerformAttack(EMeleeComboType AttackType)
{
	if (!AbilitySystemComponent) return;
	if (CombatState == ECombatState::Sheathed)
	{
		if (UnsheatheWeaponsAbility)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(UnsheatheWeaponsAbility);
		}
	}
	else if (CombatState == ECombatState::ReadyToUse)
	{
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();
		AWeapon* Weapon;
		TArray<TSubclassOf<UDungeonGameplayAbility>> WeaponAbilities;

		switch (AttackType)
		{
		case EMeleeComboType::MainHand:
			Weapon = ActiveLoadout.MainHandWeapon;
			if (!Weapon) return;
			WeaponAbilities = Weapon->GetMainHandAbilities();
			break;
		case EMeleeComboType::MainHandAlt:
			Weapon = ActiveLoadout.MainHandWeapon;
			if (!Weapon) return;
			WeaponAbilities = Weapon->GetMainHandAltAbilities();
			break;
		case EMeleeComboType::OffHand:
			Weapon = ActiveLoadout.OffHandWeapon;
			if (!Weapon) return;
			WeaponAbilities = Weapon->GetOffHandAbilities();
			break;
		case EMeleeComboType::OffHandAlt:
			Weapon = ActiveLoadout.OffHandWeapon;
			if (!Weapon) return;
			WeaponAbilities = Weapon->GetOffHandAltAbilities();
			break;
		default:
			break;
		}
	
		PerformAttack(WeaponAbilities, AttackType);
	}
}

void UPlayerCombatComponent::PerformAttack(TArray<TSubclassOf<UDungeonGameplayAbility>> Abilities, EMeleeComboType ComboType)
{
	if (!AbilitySystemComponent) return;
	if (ActiveMeleeComboType != ComboType)
	{
		// Make multicast calls first to ensure they happen on the client before proceeding
		MulticastSetActiveMeleeComboType(ComboType);
		ServerSetActiveMeleeComboType(ComboType);

		MulticastClearMeleeComboCounter();
		ServerClearMeleeComboCounter();
	}
	if (Abilities.Num() > 0)
	{
		if (ActiveMeleeComboCount >= Abilities.Num())
		{
			MulticastClearMeleeComboCounter();
			ServerClearMeleeComboCounter();
		}
		ServerSetCombatState(ECombatState::AttackInProgress);
		AbilitySystemComponent->TryActivateAbilityByClass(Abilities[ActiveMeleeComboCount]);
	}
}
