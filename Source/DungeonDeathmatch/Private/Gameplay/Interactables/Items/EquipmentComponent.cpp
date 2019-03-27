// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "Armor.h"
#include "Weapon.h"

#include "UnrealNetwork.h"
#include "RenderCaptureComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;

	bIsPrimaryLoadoutActive = true;

	Equipment = TMap<EEquipmentSlot, AEquippable*>();


	SocketNameConsumableOne = "ConsumableOne";
	SocketNameConsumableTwo = "ConsumableTwo";
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

	DOREPLIFETIME(UEquipmentComponent, bIsPrimaryLoadoutActive);
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

FName UEquipmentComponent::GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType)
{
	FName* NamePtr = WeaponSocketMap.Find(WeaponSocketType);
	if (NamePtr)
	{
		return *NamePtr;
	}

	return FName();
}

void UEquipmentComponent::ServerEquipItem_Implementation(AEquippable* Equippable, bool TryMoveReplacementToInventory /*= false*/)
{
	TArray<EEquipmentSlot> OpenEquipmentSlots = GetOpenSlotsForEquippable(Equippable);
	TArray<EEquipmentSlot> ValidEquipmentSlots = GetValidSlotsForEquippable(Equippable);
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
	// Unequip the item but don't move it back to the inventory until after the replacement item has been equipped, so we know there will be room in the inventory
	AEquippable* EquipmentInSlot = GetEquipmentInSlot(EquipmentSlot);
	if (EquipmentInSlot)
	{
		ServerUnequipItem(EquipmentInSlot, EquipmentSlot, false);
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
				WeaponToUnequip = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand));
				if (WeaponToUnequip)
				{
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneOffHand, false);
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoMainHand)
			{
				WeaponToUnequip = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand));
				if (WeaponToUnequip)
				{
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoOffHand, false);
				}
			}
		}
		else if (Weapon->GetWeaponHand() == EWeaponHand::OffHand || Weapon->GetWeaponHand() == EWeaponHand::OneHand)
		{
			if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutOneOffHand)
			{
				WeaponToUnequip = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneMainHand));
				if (WeaponToUnequip && WeaponToUnequip->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutOneMainHand, false);
				}
				else
				{
					WeaponToUnequip = nullptr;
				}
			}
			else if (EquipmentSlot == EEquipmentSlot::WeaponLoadoutTwoOffHand)
			{
				WeaponToUnequip = Cast<AWeapon>(GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoMainHand));
				if (WeaponToUnequip && WeaponToUnequip->GetWeaponHand() == EWeaponHand::TwoHand)
				{
					ServerUnequipItem(WeaponToUnequip, EEquipmentSlot::WeaponLoadoutTwoMainHand, false);
				}
				else
				{
					WeaponToUnequip = nullptr;
				}
			}
		}
	}

	bool WasItemEquipped = RequestEquipItem(Equippable, EquipmentSlot);
	if (WasItemEquipped)
	{
		// Don't "despawn" weapons since they need to be directly attached to the character mesh
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (!Weapon)
		{
			Equippable->ServerDespawn();
		}
	}

	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (InventoryComponent)
	{
		FVector DropLocation = InventoryComponent->GetItemDropLocation();

		// Now try adding any replaced equipment back to the inventory
		if (EquipmentInSlot && TryMoveReplacementToInventory)
		{
			// Spawn the item in front of the player in case the request to add it back to the inventory fails
			EquipmentInSlot->ServerSpawnAtLocation(DropLocation);
			InventoryComponent->ServerRequestAddItemToInventory(EquipmentInSlot);
		}

		// Now try adding any unequipped weapon back to the inventory
		if (WeaponToUnequip)
		{
			// Spawn the item in front of the player in case the request to add it back to the inventory fails
			WeaponToUnequip->ServerSpawnAtLocation(DropLocation);
			InventoryComponent->ServerRequestAddItemToInventory(WeaponToUnequip);
		}
	}
}

bool UEquipmentComponent::ServerEquipItemToSlot_Validate(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory /*= false*/)
{
	return true;
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

	if (GetOwner() && GetOwner()->HasAuthority())
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

void UEquipmentComponent::ServerToggleActiveLoadout_Implementation()
{
	MulticastToggleActiveLoadout();
}

bool UEquipmentComponent::ServerToggleActiveLoadout_Validate()
{
	return true;
}

bool UEquipmentComponent::IsPrimaryLoadoutActive()
{
	return bIsPrimaryLoadoutActive;
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

void UEquipmentComponent::EquipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(Slot, Equippable));
		MulticastOnItemEquipped(Equippable, Slot);
		Equippable->ServerOnEquip(OwningCharacter, Slot);
		Equippable->SetOwner(GetOwner());
	}
}


void UEquipmentComponent::UnequipItem(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Equipment.Remove(Slot);
		MulticastOnItemUnequipped(Equippable, Slot);
		Equippable->ServerOnUnequip();
		Equippable->SetOwner(nullptr);
	}
}

//void UEquipmentComponent::OnMoveToInventorySuccess(AEquippable* Equippable)
//{
//	Equippable->ServerDespawn();
//}
//
//void UEquipmentComponent::OnMoveToInventoryFail(AEquippable* Equippable)
//{
//	FVector DropLocation = FVector::ZeroVector;
//	IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(GetOwner());
//	if (InventoryInterface)
//	{
//		DropLocation = InventoryInterface->GetItemDropLocation();
//	}
//	Equippable->ServerSpawnAtLocation(DropLocation);
//}

void UEquipmentComponent::MulticastToggleActiveLoadout_Implementation()
{
	bIsPrimaryLoadoutActive = !bIsPrimaryLoadoutActive;
}

void UEquipmentComponent::MulticastOnItemEquipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Add(TTuple<EEquipmentSlot, AEquippable*>(EquipmentSlot, Equippable));

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemEquipped.Broadcast(Equippable, EquipmentSlot);
	}
}

void UEquipmentComponent::MulticastOnItemUnequipped_Implementation(AEquippable* Equippable, EEquipmentSlot EquipmentSlot)
{
	Equipment.Remove(EquipmentSlot);

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemUnequipped.Broadcast(Equippable, EquipmentSlot);
	}
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
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	AttachmentRules.bWeldSimulatedBodies = true;

	Actor->AttachToComponent(OwningCharacter->GetMesh(), AttachmentRules, SocketName);

	Actor->SetActorRelativeLocation(RelativePosition);
	Actor->SetActorRelativeRotation(RelativeRotation);

	URenderCaptureComponent* RenderCaptureComponent = Cast<URenderCaptureComponent>(OwningCharacter->GetComponentByClass(URenderCaptureComponent::StaticClass()));
	if (RenderCaptureComponent)
	{
		RenderCaptureComponent->AttachActorToSocket(Actor, SocketName, RelativePosition, RelativeRotation);
	}
}

void UEquipmentComponent::MulticastDetachActor_Implementation(AActor* Actor)
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules::KeepRelativeTransform;
	DetachRules.bCallModify = true;
	Actor->DetachFromActor(DetachRules);

	URenderCaptureComponent* RenderCaptureComponent = Cast<URenderCaptureComponent>(OwningCharacter->GetComponentByClass(URenderCaptureComponent::StaticClass()));
	if (RenderCaptureComponent)
	{
		RenderCaptureComponent->DetachActor(Actor);
	}
}
