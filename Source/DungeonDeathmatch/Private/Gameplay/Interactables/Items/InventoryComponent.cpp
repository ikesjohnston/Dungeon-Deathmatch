// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "Item.h"
#include "Equippable.h"
#include "Weapon.h"

#include "UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	bReplicates = true;

	InventoryGridSize.Row = 5;
	InventoryGridSize.Column = 6;

	DropEjectionForce = 20000;
}

// Called when the game starts
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

TArray<AItem*> UInventoryComponent::GetItems()
{
	return Items;
}

FInventoryGridPair UInventoryComponent::GetInventoryGridSize()
{
	return InventoryGridSize;
}

FVector UInventoryComponent::GetItemDropLocation()
{
	return GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * ItemDropRelativeLocation);
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
	return true;
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
	return true;
}

bool UInventoryComponent::RequestAddItem(AItem* Item)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			// Go through every grid slot and it's surrounding slots to see if the item will fit.
			// Only search columns and rows up to the point where there could still be slots to the right and below in the grid to accommodate the item size.
			FInventoryGridPair ItemSize = Item->GetGridSize();
			for (int GridRowIndex = 0; GridRowIndex < InventoryGridSize.Row; GridRowIndex++)
			{
				for (int GridColumnIndex = 0; GridColumnIndex < InventoryGridSize.Column; GridColumnIndex++)
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

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			FInventoryGridPair ItemSize = Item->GetGridSize();
			uint8 ItemRowExtent = OriginSlot.Row + ItemSize.Row;
			uint8 ItemColumnExtent = OriginSlot.Column + ItemSize.Column;

			// Go through the selected grid slots once to determine if there is more than one item in the selection area, and that it is within the grid bounds
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
	}

	return Result;
}

void UInventoryComponent::AddItem(AItem* Item, FInventoryGridPair OriginSlot)
{
	if (GetOwner() && GetOwner()->HasAuthority())
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
			}
		}

		Item->SetOwner(GetOwner());

		MulticastOnItemAdded(Item, OriginSlot);
	}
}

void UInventoryComponent::MulticastOnItemAdded_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemAdded.Broadcast(Item, OriginSlot);
	}
}

void UInventoryComponent::ServerRequestRemoveItemFromInventory_Implementation(AItem* Item)
{
	bool WasItemRemoved = RequestRemoveItem(Item);
}

bool UInventoryComponent::ServerRequestRemoveItemFromInventory_Validate(AItem* Item)
{
	return true;
}

bool UInventoryComponent::RequestRemoveItem(AItem* Item)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (int32 i = 0; i < Items.Num(); i++)
		{
			if (Items[i] == Item)
			{
				RemoveItem(i);
				Result = true;
			}
		}
	}

	return Result;
}

void UInventoryComponent::RemoveItem(int32 InventoryIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		AItem* ItemToRemove = Items[InventoryIndex];
		Items.RemoveAtSwap(InventoryIndex);

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
					GridSlot.StartingGridLocation = FInventoryGridPair(GridColumIndex, GridRowIndex);
				}
			}
		}

		ItemToRemove->SetOwner(nullptr);

		MulticastOnItemRemoved(ItemToRemove, OriginSlot);
	}
}

void UInventoryComponent::MulticastOnItemRemoved_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemRemoved.Broadcast(Item, OriginSlot);
	}
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

void UInventoryComponent::ChangeInventorySize(uint8 Rows, uint8 Columns)
{
	ServerChangeInventorySize(Rows, Columns);

	OnInventorySizeChanged.Broadcast(Rows, Columns);
}

void UInventoryComponent::ServerChangeInventorySize_Implementation(uint8 Rows, uint8 Columns)
{

}

bool UInventoryComponent::ServerChangeInventorySize_Validate(uint8 Rows, uint8 Columns)
{
	return true;
}