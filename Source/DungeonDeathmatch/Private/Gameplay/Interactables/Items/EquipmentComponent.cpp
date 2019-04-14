// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "Armor.h"
#include "Weapon.h"

#include "UnrealNetwork.h"
#include "RenderCaptureComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;
	bIsPrimaryLoadoutActive = true;
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, bIsPrimaryLoadoutActive);
}

AEquippable* UEquipmentComponent::GetEquipmentInSlot(EEquipmentSlot Slot)
{
	AEquippable* Equippable = nullptr;
	AEquippable** EquippablePtr = Equipment.Find(Slot);
	if (EquippablePtr)
	{
		Equippable =  *EquippablePtr;
	}
	return Equippable;
}

TArray<EEquipmentSlot> UEquipmentComponent::GetValidSlotsForEquippable(AEquippable* Equippable)
{
	TArray<EEquipmentSlot> ValidSlots;
	AArmor* Armor = Cast<AArmor>(Equippable);
	AWeapon* Weapon = Cast<AWeapon>(Equippable);
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
	else if (Weapon)
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

		AWeapon* MainHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneMainHand));
		if (MainHandWeapon)
		{
			if (MainHandWeapon->GetWeaponHand() == EWeaponHand::TwoHand)
			{
				ValidSlots.Remove(EEquipmentSlot::WeaponLoadoutOneOffHand);
			}
		}
		MainHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoMainHand));
		if (MainHandWeapon)
		{
			if (MainHandWeapon->GetWeaponHand() == EWeaponHand::TwoHand)
			{
				ValidSlots.Remove(EEquipmentSlot::WeaponLoadoutTwoOffHand);
			}
		}
	}
	return ValidSlots;
}

TArray<EEquipmentSlot> UEquipmentComponent::GetOpenSlotsForEquippable(AEquippable* Equippable)
{
	TArray<EEquipmentSlot> ValidSlots = GetValidSlotsForEquippable(Equippable);
	TArray<EEquipmentSlot> OpenSlots = GetOpenSlots(ValidSlots);
	return OpenSlots;
}

TArray<EEquipmentSlot>  UEquipmentComponent::GetOpenSlots(TArray<EEquipmentSlot> Slots)
{
	TArray<EEquipmentSlot> OpenSlots;
	for (EEquipmentSlot Slot : Slots)
	{
		AEquippable* EquippableInSlot = GetEquipmentInSlot(Slot);
		if (!EquippableInSlot)
		{
			OpenSlots.Add(Slot);
		}
	}
	return OpenSlots;
}

FName UEquipmentComponent::GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType)
{
	FName WeaponName = FName();
	FName* NamePtr = WeaponSocketMap.Find(WeaponSocketType);
	if (NamePtr)
	{
		WeaponName = *NamePtr;
	}
	return WeaponName;
}

void UEquipmentComponent::ServerEquipItem_Implementation(AEquippable* Equippable, bool TryMoveReplacementToInventory /*= false*/)
{
	TArray<EEquipmentSlot> ValidEquipmentSlots = GetValidSlotsForEquippable(Equippable);
	TArray<EEquipmentSlot> OpenEquipmentSlots = GetOpenSlots(ValidEquipmentSlots);
	if (OpenEquipmentSlots.Num() > 0)
	{
		EEquipmentSlot SlotToEquipItem = OpenEquipmentSlots[0];
		ServerEquipItemToSlot(Equippable, SlotToEquipItem, TryMoveReplacementToInventory);
	}
	else if (ValidEquipmentSlots.Num() > 0)
	{
		EEquipmentSlot SlotToEquipItem = ValidEquipmentSlots[0];
		ServerEquipItemToSlot(Equippable, SlotToEquipItem, TryMoveReplacementToInventory);
	}
}

bool UEquipmentComponent::ServerEquipItem_Validate(AEquippable* Equippable, bool TryMoveReplacementToInventory /*= false*/)
{
	return true;
}

void UEquipmentComponent::ServerEquipItemToSlot_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory /*= false*/)
{
	// Unequip the item but don't move it back to the inventory until after the replacement item has been equipped, so there will be additional room in the inventory
	AEquippable* EquipmentInSlot = GetEquipmentInSlot(EquipmentSlot);
	if (EquipmentInSlot)
	{
		ServerUnequipItem(EquipmentInSlot, EquipmentSlot, false);
	}

	AWeapon* Weapon = Cast<AWeapon>(Equippable);
	AWeapon* OtherHandUnequippedWeapon = GetOtherHandWeaponToUnequip(Weapon, EquipmentSlot);

	bool WasItemEquipped = RequestEquipItem(Equippable, EquipmentSlot);
	if (WasItemEquipped && !Weapon)
	{
		// Don't "despawn" weapons since they need to be directly attached to the character mesh
		Equippable->ServerDespawn();
	}

	// Now try adding any replaced equipment or additional unequipped weapon back to the inventory
	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (InventoryComponent)
	{
		FVector DropLocation = InventoryComponent->GetItemDropLocation();

		if (EquipmentInSlot && TryMoveReplacementToInventory)
		{
			// Spawn the item in front of the player in case the request to add it back to the inventory fails
			EquipmentInSlot->ServerSpawnAtLocation(DropLocation);
			InventoryComponent->ServerRequestAddItemToInventory(EquipmentInSlot);
		}

		if (OtherHandUnequippedWeapon)
		{
			OtherHandUnequippedWeapon->ServerSpawnAtLocation(DropLocation);
			InventoryComponent->ServerRequestAddItemToInventory(OtherHandUnequippedWeapon);
		}
	}
}

bool UEquipmentComponent::ServerEquipItemToSlot_Validate(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory /*= false*/)
{
	return true;
}

bool UEquipmentComponent::RequestEquipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(Slot, Equippable));
		MulticastOnItemEquipped(Equippable, Slot);
		Equippable->ServerOnEquip(GetOwner(), Slot);
		Equippable->SetOwner(GetOwner());
		Result = true;
	}

	return Result;
}

void UEquipmentComponent::MulticastOnItemEquipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(EquipmentSlot, Equippable));
	OnItemEquipped.Broadcast(Equippable, EquipmentSlot);
}

void UEquipmentComponent::ServerUnequipItem_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory /*= false*/)
{
	bool WasItemUnequipped = false;
	AEquippable* EquipmentInSlot = GetEquipmentInSlot(EquipmentSlot);
	if (EquipmentInSlot && EquipmentInSlot == Equippable)
	{
		bool WasItemUnequipped = RequestUnequipItem(Equippable, EquipmentSlot);
		if (WasItemUnequipped && TryMoveToInventory)
		{
			FVector DropLocation = GetOwner()->GetActorLocation();
			bool WasItemMovedToInventory = false;

			UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
			if (InventoryComponent)
			{
				FVector DropLocation = InventoryComponent->GetItemDropLocation();
				WasItemMovedToInventory = InventoryComponent->RequestAddItem(Equippable);
			}

			if (WasItemMovedToInventory)
			{
				Equippable->ServerDespawn();
			}
			else
			{
				Equippable->ServerSpawnAtLocation(DropLocation);
			}
		}
		else
		{
			Equippable->ServerDespawn();
		}
	}
}

bool UEquipmentComponent::ServerUnequipItem_Validate(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory /*= false*/)
{
	return true;
}

bool UEquipmentComponent::RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		AEquippable** EquippedItemPtr = Equipment.Find(Slot);
		if (EquippedItemPtr)
		{
			AEquippable* EquippedItem = *EquippedItemPtr;
			if (EquippedItem && EquippedItem == Equippable)
			{
				Equipment.Remove(Slot);
				MulticastOnItemUnequipped(Equippable, Slot);
				Equippable->ServerOnUnequip();
				Equippable->SetOwner(nullptr);
				Result = true;
			}
		}
	}

	return Result;
}

void UEquipmentComponent::MulticastOnItemUnequipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Remove(EquipmentSlot);
	OnItemUnequipped.Broadcast(Equippable, EquipmentSlot);
}

AWeapon* UEquipmentComponent::GetOtherHandWeaponToUnequip(AWeapon* Weapon, EEquipmentSlot EquipmentSlot)
{
	// If equipping a two handed weapon, unequip anything in the off hand slot. If equipping an offhand, unequip any equipped two hander in the main hand slot.
	AWeapon* WeaponToUnequip = nullptr;
	AWeapon* OccupyingWeapon = nullptr;
	if (Weapon)
	{
		if (Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
		{
			if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutOneMainHand)
			{
				OccupyingWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand));
				if (OccupyingWeapon)
				{
					WeaponToUnequip = OccupyingWeapon;
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneOffHand, false);
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoMainHand)
			{
				OccupyingWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand));
				if (OccupyingWeapon)
				{
					WeaponToUnequip = OccupyingWeapon;
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoOffHand, false);
				}
			}
		}
		else if (Weapon->GetWeaponHand() == EWeaponHand::OffHand || Weapon->GetWeaponHand() == EWeaponHand::OneHand)
		{
			if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutOneOffHand)
			{
				OccupyingWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneMainHand));
				if (OccupyingWeapon && OccupyingWeapon->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					WeaponToUnequip = OccupyingWeapon;
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneMainHand, false);
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoOffHand)
			{
				OccupyingWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoMainHand));
				if (OccupyingWeapon && OccupyingWeapon->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					WeaponToUnequip = OccupyingWeapon;
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoMainHand, false);
				}
			}
		}
	}
	return WeaponToUnequip;
}

void UEquipmentComponent::ServerToggleActiveLoadout_Implementation()
{
	bIsPrimaryLoadoutActive = !bIsPrimaryLoadoutActive;
}

bool UEquipmentComponent::ServerToggleActiveLoadout_Validate()
{
	return true;
}

FWeaponLoadout UEquipmentComponent::GetActiveWeaponLoadout()
{
	AWeapon* MainHandWeapon = nullptr;
	AWeapon* OffHandWeapon = nullptr;

	if (bIsPrimaryLoadoutActive)
	{
		MainHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneMainHand));
		OffHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand));
	}
	else
	{
		MainHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoMainHand));
		OffHandWeapon = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand));
	}

	FWeaponLoadout Loadout = FWeaponLoadout(MainHandWeapon, OffHandWeapon);
	return Loadout;
}

void UEquipmentComponent::ServerAttachActorToSocket_Implementation(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	MulticastAttachActorToSocket(Actor, SocketName, RelativePosition, RelativeRotation);
}

bool UEquipmentComponent::ServerAttachActorToSocket_Validate(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	return true;
}

void UEquipmentComponent::ServerDetachActor_Implementation(AActor* Actor)
{
	MulticastDetachActor(Actor);
}

bool UEquipmentComponent::ServerDetachActor_Validate(AActor* Actor)
{
	return true;
}

void UEquipmentComponent::MulticastAttachActorToSocket_Implementation(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	if (!Actor) return;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	AttachmentRules.bWeldSimulatedBodies = true;

	AItem* Item = Cast<AItem>(Actor);
	if (Item)
	{
		UMeshComponent* RootMesh = Item->GetRootMeshComponent();
		if (RootMesh)
		{
			RootMesh->SetSimulatePhysics(false);
		}
		Item->Execute_OnUnfocused(Item);
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Actor->AttachToComponent(Character->GetMesh(), AttachmentRules, SocketName);
		Actor->SetActorRelativeLocation(RelativePosition);
		Actor->SetActorRelativeRotation(RelativeRotation);
	}

	URenderCaptureComponent* RenderCaptureComponent = Cast<URenderCaptureComponent>(GetOwner()->GetComponentByClass(URenderCaptureComponent::StaticClass()));
	if (RenderCaptureComponent)
	{
		RenderCaptureComponent->AttachActorToSocket(Actor, SocketName, RelativePosition, RelativeRotation);
	}
}

void UEquipmentComponent::MulticastDetachActor_Implementation(AActor* Actor)
{
	if (!Actor) return;

	FDetachmentTransformRules DetachRules = FDetachmentTransformRules::KeepRelativeTransform;
	DetachRules.bCallModify = true;
	Actor->DetachFromActor(DetachRules);

	URenderCaptureComponent* RenderCaptureComponent = Cast<URenderCaptureComponent>(GetOwner()->GetComponentByClass(URenderCaptureComponent::StaticClass()));
	if (RenderCaptureComponent)
	{
		RenderCaptureComponent->DetachActor(Actor);
	}
}
