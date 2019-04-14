// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "Item.h"
#include "Equippable.h"
#include "Weapon.h"

#include <DrawDebugHelpers.h>

UInventoryComponent::UInventoryComponent()
{
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;

	InventoryGridSize.Row = 5;
	InventoryGridSize.Column = 6;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryGrid = TArray<FInventoryGridSlot>();
	InventoryGrid.AddDefaulted(InventoryGridSize.Row * InventoryGridSize.Column);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);
	DOREPLIFETIME(UInventoryComponent, InventoryGrid);
	DOREPLIFETIME(UInventoryComponent, InventoryGridSize);
}

FVector UInventoryComponent::GetItemDropLocation()
{
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorRotation().RotateVector(ItemDropRelativeLocation);
}

void UInventoryComponent::ServerRequestAddItemToInventory_Implementation(AItem* Item)
{
	bool WasItemAdded = RequestAddItem(Item);
	if (WasItemAdded)
	{
		Item->ServerDespawn();
	}
}

bool UInventoryComponent::ServerRequestAddItemToInventory_Validate(AItem* Item)
{
	bool Result = ValidateItem(Item);
	return Result;
}

void UInventoryComponent::ServerRequestAddItemToInventoryAtLocation_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	bool WasItemAdded = RequestAddItem(Item, OriginSlot);
	if (WasItemAdded)
	{
		Item->ServerDespawn();
	}
}

bool UInventoryComponent::ServerRequestAddItemToInventoryAtLocation_Validate(AItem* Item, FInventoryGridPair OriginSlot)
{
	bool Result = ValidateItem(Item);
	return Result;
}

bool UInventoryComponent::RequestAddItem(AItem* Item)
{
	bool Result = false;

	if (Item && GetOwner() && GetOwner()->HasAuthority())
	{
		// Go through every grid slot and it's surrounding slots to see if the item will fit.
		FInventoryGridPair ItemSize = Item->GetGridSize();
		if (ItemSize.Row <= InventoryGridSize.Row && ItemSize.Column <= InventoryGridSize.Column)
		{
			for (int GridRowIndex = 0; GridRowIndex <= InventoryGridSize.Row; GridRowIndex++)
			{
				for (int GridColumnIndex = 0; GridColumnIndex <= InventoryGridSize.Column; GridColumnIndex++)
				{
					FInventoryGridPair OriginSlot = FInventoryGridPair(GridRowIndex, GridColumnIndex);
					Result = RequestAddItem(Item, OriginSlot);
					if (Result)
					{
						break;
					}
				}
				if (Result)
				{
					break;
				}
			}
		}
	}

	return Result;
}

bool UInventoryComponent::RequestAddItem(AItem* Item, FInventoryGridPair OriginSlot)
{
	bool Result = true;

	if (Item && GetOwner() && GetOwner()->HasAuthority())
	{
		FInventoryGridPair ItemSize = Item->GetGridSize();
		uint8 ItemRowExtent = OriginSlot.Row + ItemSize.Row;
		uint8 ItemColumnExtent = OriginSlot.Column + ItemSize.Column;

		// Go through the selected grid slots once to determine if there is more than one item in the selection area, and if it is within the grid bounds
		for (int GridRowIndex = OriginSlot.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
		{
			for (int GridColumnIndex = OriginSlot.Column; GridColumnIndex < ItemColumnExtent; GridColumnIndex++)
			{
				uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumnIndex;
				if (GridSlotIndex >= 0 && GridSlotIndex < InventoryGrid.Num())
				{
					FInventoryGridSlot& GridSlot = InventoryGrid[GridSlotIndex];
					if (GridSlot.Item)
					{
						Result = false;
						break;
					}
				}
				else
				{
					Result = false;
					break;
				}
			}
			if (!Result)
			{
				break;
			}
		}

		if (Result)
		{
			AddItem(Item, OriginSlot);
		}
	}

	return Result;
}

void UInventoryComponent::AddItem(AItem* Item, FInventoryGridPair &OriginSlot)
{
	Items.Add(Item);

	// Update the grid data
	FInventoryGridPair ItemSize = Item->GetGridSize();
	uint8 ItemRowExtent = OriginSlot.Row + ItemSize.Row;
	uint8 ItemColumnExtent = OriginSlot.Column + ItemSize.Column;

	for (int GridRowIndex = OriginSlot.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
	{
		for (int GridColumIndex = OriginSlot.Column; GridColumIndex < ItemColumnExtent; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			FInventoryGridSlot& GridSlot = InventoryGrid[GridSlotIndex];
			GridSlot.Item = Item;
			GridSlot.ItemOriginGridLocation = OriginSlot;
		}
	}

	Item->SetOwner(GetOwner());
	MulticastOnItemAdded(Item, OriginSlot);
}

void UInventoryComponent::MulticastOnItemAdded_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	OnItemAdded.Broadcast(Item, OriginSlot);
}

void UInventoryComponent::ServerRequestRemoveItemFromInventory_Implementation(AItem* Item)
{
	bool WasItemRemoved = RequestRemoveItem(Item);
}

bool UInventoryComponent::ServerRequestRemoveItemFromInventory_Validate(AItem* Item)
{
	bool Result = ValidateItem(Item);
	return Result;
}

bool UInventoryComponent::RequestRemoveItem(AItem* Item)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++)
		{
			if (Items[ItemIndex] == Item)
			{
				RemoveItem(ItemIndex);
				Result = true;
				break;
			}
		}
	}

	return Result;
}

void UInventoryComponent::RemoveItem(int32 ItemIndex)
{
	AItem* ItemToRemove = Items[ItemIndex];
	Items.RemoveAtSwap(ItemIndex);

	// Find first slot containing the item, this will be the origin to broadcast
	FInventoryGridPair OriginSlot;
	bool OriginFound = false;
	for (int GridRowIndex = 0; GridRowIndex < InventoryGridSize.Row; GridRowIndex++)
	{
		for (int GridColumIndex = 0; GridColumIndex < InventoryGridSize.Column; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			FInventoryGridSlot& GridSlot = InventoryGrid[GridSlotIndex];
			if (GridSlot.Item && GridSlot.Item == ItemToRemove)
			{
				if (!OriginFound)
				{
					OriginSlot = FInventoryGridPair(GridColumIndex, GridRowIndex);
					OriginFound = true;
				}
				GridSlot.Item = nullptr;
				GridSlot.ItemOriginGridLocation = FInventoryGridPair(GridColumIndex, GridRowIndex);
			}
		}
	}

	ItemToRemove->SetOwner(nullptr);
	MulticastOnItemRemoved(ItemToRemove, OriginSlot);
}

bool UInventoryComponent::ValidateItem(AItem* Item)
{
	bool Result = true;
	FInventoryGridPair ItemSize = Item->GetGridSize();
	if (ItemSize.Row <= 0 || ItemSize.Column <= 0)
	{
		Result = false;
	}
	return Result;
}

void UInventoryComponent::MulticastOnItemRemoved_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	OnItemRemoved.Broadcast(Item, OriginSlot);
}

void UInventoryComponent::ServerRequestPickUpItem_Implementation(AItem* Item)
{
	bool WasItemAdded = false;
	bool WasItemEquipped = false;

	// Try equipping the item first, if there is an appropriate open slot
	AEquippable* Equippable = Cast<AEquippable>(Item);
	UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwner()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (Equippable && EquipmentComponent)
	{
		TArray<EEquipmentSlot> OpenEquipmentSlots = EquipmentComponent->GetOpenSlotsForEquippable(Equippable);

		// If trying to equip a two handed weapon, make sure both main and off hand slots are open
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (Weapon)
		{
			if (Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
			{
				if (EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutOneOffHand))
				{
					OpenEquipmentSlots.Remove(EEquipmentSlot::WeaponLoadoutOneMainHand);
				}
				if (EquipmentComponent->GetEquipmentInSlot(EEquipmentSlot::WeaponLoadoutTwoOffHand))
				{
					OpenEquipmentSlots.Remove(EEquipmentSlot::WeaponLoadoutTwoMainHand);
				}
			}
		}

		if (OpenEquipmentSlots.Num() > 0)
		{
			EEquipmentSlot SlotToEquipItem = OpenEquipmentSlots[0];

			WasItemEquipped = EquipmentComponent->RequestEquipItem(Equippable, SlotToEquipItem);
		}
	}

	if (WasItemEquipped)
	{
		// Don't "despawn" weapons since they need to be directly attached to the character mesh
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (!Weapon)
		{
			Equippable->ServerDespawn();
		}
	}
	else
	{
		WasItemAdded = RequestAddItem(Item);
	}

	if (WasItemAdded)
	{
		Item->ServerDespawn();
	}
}

bool UInventoryComponent::ServerRequestPickUpItem_Validate(AItem* Item)
{
	return true;
}

void UInventoryComponent::ServerRequestDropItem_Implementation(AItem* Item, bool CheckInventory /*= true*/)
{
	bool WasItemRemoved = true;

	if (CheckInventory)
	{
		WasItemRemoved = RequestRemoveItem(Item);
	}

	if (WasItemRemoved)
	{
		Item->ServerSpawnAtLocation(GetItemDropLocation());
	}
}

bool UInventoryComponent::ServerRequestDropItem_Validate(AItem* Item, bool CheckInventory /*= true*/)
{
	return true;
}