// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"
#include "Armor.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;

	bIsPrimaryLoadoutActive = true;

	EquippedArmor = TMap<EArmorSlot, AArmor*>();
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

	DOREPLIFETIME(UEquipmentComponent, EquippedArmor);
	DOREPLIFETIME(UEquipmentComponent, PrimaryWeaponLoadout);
	DOREPLIFETIME(UEquipmentComponent, SecondaryWeaponLoadout);

}

TMap<EArmorSlot, AArmor*> UEquipmentComponent::GetArmor() const
{
	return EquippedArmor;
}

AArmor* UEquipmentComponent::GetArmorInSlot(EArmorSlot Slot)
{
	return *(EquippedArmor.Find(Slot));
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
//
//void UEquipmentComponent::Multicast_OnEquipItem_Implementation(AEquippable* Equippable)
//{
//	Equippable->GetMeshComponent()->SetVisibility(true);
//
//	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
//	AttachRules.bWeldSimulatedBodies = false;
//
//	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
//	if (OwningCharacter)
//	{
//		Equippable->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, "HipSheatheSocket");
//	}
//}
//
//void UEquipmentComponent::Server_EquipItem_Implementation(AEquippable* Equippable)
//{
////	if (!HasItemEquipped(Equippable))
////	{
////		Equipment.Insert(Equippable, (int32) EEquipmentSlot::MainHand);
////		Multicast_OnEquipItem(Equippable);
////		// test code, to remove 
////		ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
////		if (Character)
////		{
//////				Item->Server_OnEquip(Character);
////		}
////	}
//}
//
//bool UEquipmentComponent::Server_EquipItem_Validate(AEquippable* Equippable)
//{
//	return true;
//}
//
//void UEquipmentComponent::Server_UnequipItem_Implementation(AEquippable* Equippable)
//{
//	/*if (!HasItemEquipped(Equippable))
//	{
//		Equipment.RemoveSwap(Equippable);
//		Multicast_OnUnequipItem(Equippable);
//		OnItemUnequipped.ExecuteIfBound(Equippable);
//	}*/
//}
//
//bool UEquipmentComponent::Server_UnequipItem_Validate(AEquippable* Equippable)
//{
//	return true;
//}
//
//void UEquipmentComponent::Multicast_OnUnequipItem_Implementation(AEquippable* Equippable)
//{
//	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
//	Equippable->DetachFromActor(DetachRules);
//}
//
//void UEquipmentComponent::Server_UnequipAll_Implementation()
//{
//	/*while (Equipment.Num() > 0)
//	{
//		Server_UnequipItem(Equipment[0]);
//	}*/
//}
//
//bool UEquipmentComponent::Server_UnequipAll_Validate()
//{
//	return true;
//}

bool UEquipmentComponent::RequestEquipItem(AEquippable* Equippable, bool CanReplaceEquipment /*= false*/)
{
	bool WasItemEquipped = false;

	if (GetOwner()->Role == ROLE_Authority)
	{
		// First, determine what kind of item is being requested to equip
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (Weapon)
		{
			WasItemEquipped = RequestEquipWeapon(Weapon, CanReplaceEquipment);
		}
		AArmor* Armor = Cast<AArmor>(Equippable);
		if (Armor)
		{
			EArmorSlot ArmorSlot = Armor->GetArmorSlot();
			//AArmor* ArmorInSlot = GetArmorInSlot(ArmorSlot);
			//if (!ArmorInSlot)
			//{
				TPair<EArmorSlot, AArmor*> Pair = TPair<EArmorSlot, AArmor*>(ArmorSlot, Armor);
				EquippedArmor.Add(Pair);
				WasItemEquipped = true;
			//}
		}
	}

	if (WasItemEquipped)
	{
		Equippable->OnEquip(OwningCharacter);
	}

	return WasItemEquipped;
}

bool UEquipmentComponent::RequestEquipWeapon(AWeapon* Weapon, bool CanReplaceEquipment /*= false*/)
{
	bool WasWeaponEquipped = false;

	FWeaponLoadout& ActiveLoadout = GetActiveLoadout();
	FWeaponLoadout& InactiveLoadout = GetInactiveLoadout();

	if (GetOwner()->Role == ROLE_Authority)
	{
		switch (Weapon->GetWeaponHand())
		{
		case EWeaponHand::OneHand:
			// First, try to find an open slot
			if (!ActiveLoadout.MainHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
			}
			else if (!ActiveLoadout.OffHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
			}
			else if(!InactiveLoadout.MainHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::MainHand);
			}
			else if (!InactiveLoadout.OffHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::OffHand);
			}
			// If no slots are open, just replace the main hand of the active loadout
			if (!WasWeaponEquipped)
			{
				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::MainHand);
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
			}
			break;
		case EWeaponHand::TwoHand:
			// First, try to find an open slot
			if (!ActiveLoadout.MainHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
			}
			else if (!InactiveLoadout.MainHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::MainHand);
			}
			// If no slots are open, just replace the main hand of the active loadout
			if (!WasWeaponEquipped)
			{
				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::MainHand);
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
			}
			break;
		case EWeaponHand::OffHand:
			// First, try to find an open slot
			 if (!ActiveLoadout.OffHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
			}
			else if (!InactiveLoadout.OffHandWeapon)
			{
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::OffHand);
			}
			// If no slots are open, just replace the off hand of the active loadout
			if (!WasWeaponEquipped)
			{
				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::OffHand);
				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
			}
			break;
		default:
			break;
		}
	}

	return WasWeaponEquipped;
}

bool UEquipmentComponent::RequestAddWeaponToLoadout(AWeapon* Weapon, FWeaponLoadout& Loadout, ERequestedHand RequestedHand)
{
	bool WasWeaponAdded = false;

	if (GetOwner()->Role == ROLE_Authority)
	{
		switch (Weapon->GetWeaponHand())
		{
		case EWeaponHand::OneHand:
			break;
		case EWeaponHand::TwoHand:
			break;
		case EWeaponHand::OffHand:
			break;
		default:
			break;
		}
	}

	return WasWeaponAdded;
}

bool UEquipmentComponent::RequestUnequipItem(AEquippable* Equippable)
{
	return true;
}

bool UEquipmentComponent::RequestRemoveWeaponFromLoadout(FWeaponLoadout& Loadout, ERequestedHand RequestedHand)
{
	if (GetOwner()->Role == ROLE_Authority)
	{
		AWeapon* WeaponToRemove = nullptr;
		switch (RequestedHand)
		{
		case ERequestedHand::MainHand:
			WeaponToRemove = Loadout.MainHandWeapon;
			if (WeaponToRemove)
			{
				WeaponToRemove->OnUnequip();
				Loadout.MainHandWeapon = nullptr;
			}
			break;
		case ERequestedHand::OffHand:
			WeaponToRemove = Loadout.OffHandWeapon;
			if (WeaponToRemove)
			{
				WeaponToRemove->OnUnequip();
				Loadout.OffHandWeapon = nullptr;
			}
			break;
		case ERequestedHand::Both:
			WeaponToRemove = Loadout.MainHandWeapon;
			if (WeaponToRemove)
			{
				WeaponToRemove->OnUnequip();
				Loadout.MainHandWeapon = nullptr;
			}
			WeaponToRemove = Loadout.OffHandWeapon;
			if (WeaponToRemove)
			{
				WeaponToRemove->OnUnequip();
				Loadout.OffHandWeapon = nullptr;
			}
			break;
		default:
			break;
		}
	}

	return true;
}

void UEquipmentComponent::EquipItem(AEquippable* Equippable, bool CanReplaceEquipment /*= false*/)
{

}

void UEquipmentComponent::EquipWeapon(AWeapon* Weapon, bool CanReplaceEquipment /*= false*/)
{

}

void UEquipmentComponent::UnequipItem(AEquippable* Equippable)
{

}

void UEquipmentComponent::AddWeaponToLoadout(AWeapon* Weapon, FWeaponLoadout& Loadout, ERequestedHand RequestedHand)
{

}

void UEquipmentComponent::RemoveWeaponFromLoadout(FWeaponLoadout& Loadout, ERequestedHand RequestedHand)
{

}

void UEquipmentComponent::Multicast_OnItemUnequipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{

}

void UEquipmentComponent::Multicast_OnItemEquipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{

}
