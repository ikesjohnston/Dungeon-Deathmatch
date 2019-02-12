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

bool UInventoryComponent::RequestAddItem(AItem* Item)
{
	bool Result = false;
	
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
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

	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
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

bool UInventoryComponent::RequestRemoveItem(AItem* Item)
{
	bool Result = false;

	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
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

void UInventoryComponent::ChangeInventorySize(uint8 Rows, uint8 Columns)
{
	Server_ChangeInventorySize(Rows, Columns);

	OnInventorySizeChanged.Broadcast(Rows, Columns);
}

void UInventoryComponent::AddItem(AItem* Item, FInventoryGridPair OriginSlot)
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
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

		Multicast_OnItemAdd(Item, OriginSlot);
	}
}
void UInventoryComponent::RemoveItem(int32 InventoryIndex)
{
	if (GetOwner() && GetOwner()->Role == ROLE_Authority)
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

		Multicast_OnItemRemove(ItemToRemove, OriginSlot);
	}
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

}

bool UInventoryComponent::Server_ChangeInventorySize_Validate(uint8 Rows, uint8 Columns)
{
	return true;
}