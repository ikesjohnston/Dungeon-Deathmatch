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

	DOREPLIFETIME(UEquipmentComponent, Equipment);
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

//bool UEquipmentComponent::RequestEquipItem(AEquippable* Equippable, bool CanReplaceEquipment /*= false*/)
//{
//	bool WasItemEquipped = false;
//
//	if (GetOwner()->Role == ROLE_Authority)
//	{
//		// First, determine what kind of item is being requested to equip
//		AWeapon* Weapon = Cast<AWeapon>(Equippable);
//		if (Weapon)
//		{
//			WasItemEquipped = RequestEquipWeapon(Weapon, CanReplaceEquipment);
//		}
//		AArmor* Armor = Cast<AArmor>(Equippable);
//		if (Armor)
//		{
//			EArmorSlot ArmorSlot = Armor->GetArmorSlot();
//			//AArmor* ArmorInSlot = GetArmorInSlot(ArmorSlot);
//			//if (!ArmorInSlot)
//			//{
//				/*TPair<EArmorSlot, AArmor*> Pair = TPair<EArmorSlot, AArmor*>(ArmorSlot, Armor);
//				EquippedArmor.Add(Pair);
//				WasItemEquipped = true;*/
//			//}
//		}
//	}
//
//	if (WasItemEquipped)
//	{
//		Equippable->OnEquip(OwningCharacter);
//	}
//
//	return WasItemEquipped;
//}
//
//bool UEquipmentComponent::RequestEquipWeapon(AWeapon* Weapon, bool CanReplaceEquipment /*= false*/)
//{
//	bool WasWeaponEquipped = false;
//
//	FWeaponLoadout& ActiveLoadout = GetActiveLoadout();
//	FWeaponLoadout& InactiveLoadout = GetInactiveLoadout();
//
//	if (GetOwner()->Role == ROLE_Authority)
//	{
//		switch (Weapon->GetWeaponHand())
//		{
//		case EWeaponHand::OneHand:
//			// First, try to find an open slot
//			if (!ActiveLoadout.MainHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
//			}
//			else if (!ActiveLoadout.OffHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
//			}
//			else if(!InactiveLoadout.MainHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::MainHand);
//			}
//			else if (!InactiveLoadout.OffHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::OffHand);
//			}
//			// If no slots are open, just replace the main hand of the active loadout
//			if (!WasWeaponEquipped)
//			{
//				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::MainHand);
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
//			}
//			break;
//		case EWeaponHand::TwoHand:
//			// First, try to find an open slot
//			if (!ActiveLoadout.MainHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
//			}
//			else if (!InactiveLoadout.MainHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::MainHand);
//			}
//			// If no slots are open, just replace the main hand of the active loadout
//			if (!WasWeaponEquipped)
//			{
//				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::MainHand);
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::MainHand);
//			}
//			break;
//		case EWeaponHand::OffHand:
//			// First, try to find an open slot
//			 if (!ActiveLoadout.OffHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
//			}
//			else if (!InactiveLoadout.OffHandWeapon)
//			{
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, InactiveLoadout, ERequestedHand::OffHand);
//			}
//			// If no slots are open, just replace the off hand of the active loadout
//			if (!WasWeaponEquipped)
//			{
//				RequestRemoveWeaponFromLoadout(ActiveLoadout, ERequestedHand::OffHand);
//				WasWeaponEquipped = RequestAddWeaponToLoadout(Weapon, ActiveLoadout, ERequestedHand::OffHand);
//			}
//			break;
//		default:
//			break;
//		}
//	}
//
//	return WasWeaponEquipped;
//}

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
		Equippable->Server_OnEquip(OwningCharacter);
		Multicast_OnItemEquipped(Equippable, Slot);
	}
}


void UEquipmentComponent::UnequipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		Equipment.Remove(Slot);
		Equippable->Server_OnUnequip();

		Multicast_OnItemUnequipped(Equippable, Slot);
	}
}

void UEquipmentComponent::Multicast_OnItemEquipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemEquipped.Broadcast(Equippable, EquipmentSlot);
	}
}

void UEquipmentComponent::Multicast_OnItemUnequipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemUnequipped.Broadcast(Equippable, EquipmentSlot);
	}
}
