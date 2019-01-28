// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"
#include "Armor.h"
#include "Weapon.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;

	bIsPrimaryLoadoutActive = true;

	Equipment = TMap<EEquipmentSlot, AEquippable*>();
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	ADungeonCharacter* DungeonCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (DungeonCharacter)
	{
		OwningCharacter = DungeonCharacter;
	}
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, PrimaryWeaponLoadout);
	DOREPLIFETIME(UEquipmentComponent, SecondaryWeaponLoadout);

}

TMap<EEquipmentSlot, AEquippable*> UEquipmentComponent::GetEquipment() const
{
	return Equipment;
}

AEquippable* UEquipmentComponent::GetEquipmentInSlot(EEquipmentSlot Slot)
{
	AEquippable** EquipmentPtr = Equipment.Find(Slot);
	if (EquipmentPtr)
	{
		return *EquipmentPtr;
	}

	return nullptr;
}

TArray<EEquipmentSlot> UEquipmentComponent::GetValidSlotsForEquippable(AEquippable* Equippable)
{
	TArray<EEquipmentSlot> ValidSlots;

	AArmor* Armor = Cast<AArmor>(Equippable);
	if (Armor)
	{
		switch (Armor->GetArmorSlot())
		{
		case EArmorSlot::Head:
			ValidSlots.Add(EEquipmentSlot::Head);
			break;
		case EArmorSlot::Neck:
			ValidSlots.Add(EEquipmentSlot::Neck);
			break;
		case EArmorSlot::Shoulders:
			ValidSlots.Add(EEquipmentSlot::Shoulders);
			break;
		case EArmorSlot::Chest:
			ValidSlots.Add(EEquipmentSlot::Chest);
			break;
		case EArmorSlot::Hands:
			ValidSlots.Add(EEquipmentSlot::Hands);
			break;
		case EArmorSlot::Finger:
			ValidSlots.Add(EEquipmentSlot::FingerOne);
			ValidSlots.Add(EEquipmentSlot::FingerTwo);
			break;
		case EArmorSlot::Waist:
			ValidSlots.Add(EEquipmentSlot::Waist);
			break;
		case EArmorSlot::Legs:
			ValidSlots.Add(EEquipmentSlot::Legs);
			break;
		case EArmorSlot::Feet:
			ValidSlots.Add(EEquipmentSlot::Feet);
			break;
		default:
			break;
		}
	}

	AWeapon* Weapon = Cast<AWeapon>(Equippable);
	if (Weapon)
	{
		switch (Weapon->GetWeaponHand())
		{
		case EWeaponHand::OneHand:
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutOneMainHand);
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutOneOffHand);
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutTwoMainHand);
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutTwoOffHand);
			break;
		case EWeaponHand::TwoHand:
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutOneMainHand);
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutTwoMainHand);
			break;
		case EWeaponHand::OffHand:
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutOneOffHand);
			ValidSlots.Add(EEquipmentSlot::WeaponLoadoutTwoOffHand);
			break;
		default:
			break;
		}
	}

	return ValidSlots;
}

TArray<EEquipmentSlot> UEquipmentComponent::GetOpenSlotsForEquippable(AEquippable* Equippable)
{
	TArray<EEquipmentSlot> OpenSlots;
	TArray<EEquipmentSlot> ValidSlots = GetValidSlotsForEquippable(Equippable);
	for (EEquipmentSlot Slot : ValidSlots)
	{
		AEquippable* EquippableInSlot = GetEquipmentInSlot(Slot);
		if (!EquippableInSlot)
		{
			OpenSlots.Add(Slot);
		}
	}

	return OpenSlots;
}

FWeaponLoadout& UEquipmentComponent::GetActiveLoadout()
{
	if (bIsPrimaryLoadoutActive)
	{
		return PrimaryWeaponLoadout;
	}
	return SecondaryWeaponLoadout;
}

FWeaponLoadout& UEquipmentComponent::GetInactiveLoadout()
{
	if (bIsPrimaryLoadoutActive)
	{
		return SecondaryWeaponLoadout;
	}
	return PrimaryWeaponLoadout;
}

bool UEquipmentComponent::RequestEquipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	bool Result = false;

	EquipItem(Equippable, Slot);
	Result = true;

	return Result;
}

bool UEquipmentComponent::RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		AEquippable** EquippedItemPtr = Equipment.Find(Slot);
		if (EquippedItemPtr)
		{
			AEquippable* EquippedItem = *EquippedItemPtr;
			if (EquippedItem && EquippedItem == Equippable)
			{
				UnequipItem(Equippable, Slot);
				Result = true;
			}
		}
	}

	return Result;
}

void UEquipmentComponent::EquipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(Slot, Equippable));
		Multicast_OnItemEquipped(Equippable, Slot);
		Equippable->Server_OnEquip(OwningCharacter);
	}
}


void UEquipmentComponent::UnequipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		Equipment.Remove(Slot);
		Multicast_OnItemUnequipped(Equippable, Slot);
		Equippable->Server_OnUnequip();
	}
}

void UEquipmentComponent::Multicast_OnItemEquipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(EquipmentSlot, Equippable));

	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemEquipped.Broadcast(Equippable, EquipmentSlot);
	}
}

void UEquipmentComponent::Multicast_OnItemUnequipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Remove(EquipmentSlot);

	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemUnequipped.Broadcast(Equippable, EquipmentSlot);
	}
}
