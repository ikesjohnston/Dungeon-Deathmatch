// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "Item.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	bReplicates = true;

	InventoryGridSize.Row = 5;
	InventoryGridSize.Column = 6;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryGrid = TArray<FInventoryGridSlot>();
	InventoryGrid.AddDefaulted(InventoryGridSize.Row * InventoryGridSize.Column);

	InitializeGrid();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryItems);
	DOREPLIFETIME(UInventoryComponent, InventoryGrid);
	DOREPLIFETIME(UInventoryComponent, InventoryGridSize);
}


void UInventoryComponent::InitializeGrid()
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
	{
		uint8 NumberOfGridSlots = InventoryGridSize.Row * InventoryGridSize.Column;
		for (int GridSlotIndex = 0; GridSlotIndex < NumberOfGridSlots; GridSlotIndex++)
		{
			FInventoryGridSlot& GridSlot = InventoryGrid[GridSlotIndex];
			GridSlot.NumberOfOpenSlotsRight = 0;
			GridSlot.NumberOfOpenSlotsBelow = 0;
			uint8 SlotColumnIndex = (GridSlotIndex % InventoryGridSize.Column);
			uint8 SlotRowIndex = (GridSlotIndex - SlotColumnIndex) / InventoryGridSize.Column;

			// Start search at the next adjacent row and column
			uint8 AdjacentColumnIndex = SlotColumnIndex + 1;
			uint8 AdjacentRowIndex = SlotRowIndex + 1;

			// Check slots to the right (on the same row)
			for (AdjacentColumnIndex; AdjacentColumnIndex < InventoryGridSize.Column; AdjacentColumnIndex++)
			{
				FInventoryGridSlot& AdjacentSlot = InventoryGrid[AdjacentColumnIndex + (SlotRowIndex * InventoryGridSize.Column)];
				if (AdjacentSlot.Item)
				{
					break;
				}
				GridSlot.NumberOfOpenSlotsRight++;
			}

			// Check slots below (on the same column)
			for (AdjacentRowIndex; AdjacentRowIndex < InventoryGridSize.Row; AdjacentRowIndex++)
			{
				FInventoryGridSlot& AdjacentSlot = InventoryGrid[SlotColumnIndex + (AdjacentRowIndex * InventoryGridSize.Column)];
				if (AdjacentSlot.Item)
				{
					break;
				}
				GridSlot.NumberOfOpenSlotsBelow++;
			}
		}
	}
}


TArray<AItem*> UInventoryComponent::GetInventory()
{
	return InventoryItems;
}

FInventoryGridPair UInventoryComponent::GetInventoryGridSize()
{
	return InventoryGridSize;
}

bool UInventoryComponent::TryAddItem(AItem* Item)
{
	bool Result = false;
	if (Item)
	{
		// Search through the grid until an open space is found or the entire grid has been searched.
		// Only search columns and rows up to the point where there could still be slots to the right and below in the grid to accommodate the item size.
		FInventoryGridPair ItemSize = Item->GetGridSize();
		for (int GridRowIndex = 0; GridRowIndex < InventoryGridSize.Row - (ItemSize.Row - 1); GridRowIndex++)
		{
			for (int GridColumIndex = 0; GridColumIndex < InventoryGridSize.Column - (ItemSize.Column - 1); GridColumIndex++)
			{
				uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
				FInventoryGridSlot& GridSlot = InventoryGrid[GridSlotIndex];
				if (!GridSlot.Item && (GridSlot.NumberOfOpenSlotsRight >= ItemSize.Column - 1) && (GridSlot.NumberOfOpenSlotsBelow >= ItemSize.Row - 1))
				{
					FInventoryGridPair OriginSlot = FInventoryGridPair(GridColumIndex, GridRowIndex);
					Server_AddItem(Item, OriginSlot);
					Result = true;
					break;
				}
			}
			if (Result)
			{
				break;
			}
		}
	}

	return Result;
}

bool UInventoryComponent::TryRemoveItem(AItem* Item)
{
	bool Result = false;
	for (int32 i = 0; i < InventoryItems.Num(); i++)
	{
		if (InventoryItems[i] == Item)
		{
			Server_RemoveItem(i);
			Result = true;
		}
	}
	return Result;
}

void UInventoryComponent::ChangeInventorySize(uint8 Rows, uint8 Columns)
{
	Server_ChangeInventorySize(Rows, Columns);

	OnInventorySizeChanged.Broadcast(Rows, Columns);
}

void UInventoryComponent::Server_AddItem_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	InventoryItems.Add(Item);
	
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

	// Refresh adjacent open slot counts
	InitializeGrid();

	Multicast_OnItemAdd(Item, OriginSlot);
}

bool UInventoryComponent::Server_AddItem_Validate(AItem* Item, FInventoryGridPair OriginSlot)
{
	return true;
}

void UInventoryComponent::Server_RemoveItem_Implementation(int32 InventoryIndex)
{
	AItem* ItemToRemove = InventoryItems[InventoryIndex];
	InventoryItems.RemoveAtSwap(InventoryIndex);

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

	// Refresh adjacent open slot counts
	InitializeGrid();

	Multicast_OnItemRemove(ItemToRemove, OriginSlot);
}

bool UInventoryComponent::Server_RemoveItem_Validate(int32 InventoryIndex)
{
	return true;
}

void UInventoryComponent::Multicast_OnItemAdd_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemAdded.Broadcast(Item, OriginSlot);
	}
}

void UInventoryComponent::Multicast_OnItemRemove_Implementation(AItem* Item, FInventoryGridPair OriginSlot)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemRemoved.Broadcast(Item, OriginSlot);
	}
}

void UInventoryComponent::Server_ChangeInventorySize_Implementation(uint8 Rows, uint8 Columns)
{
	InitializeGrid();
}

bool UInventoryComponent::Server_ChangeInventorySize_Validate(uint8 Rows, uint8 Columns)
{
	return true;
}