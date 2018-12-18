// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEquipmentMenuWidget.h"
#include "InventoryEquipmentSlotWidget.h"
#include <GridPanel.h>
#include <UserWidget.h>
#include <GridSlot.h>
#include "InventoryComponent.h"
#include "DungeonPlayerController.h"

UInventoryEquipmentMenuWidget::UInventoryEquipmentMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridPanelName = "Grid_Inventory";

	NumberOfInventorySlots = 16;
	NumberOfGridColumns = 4;

	GridSlotPadding = 1.0f;

	//InitializeGrid();
}

bool UInventoryEquipmentMenuWidget::Initialize()
{
	bool Result = Super::Initialize();
	// Get the player's inventory component and bind item add and remove events
	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(GetOwningPlayerPawn());
	if (OwningCharacter)
	{
		UInventoryComponent* InventoryComp = OwningCharacter->GetInventoryComponent();
		if (InventoryComp)
		{
			InventoryComp->OnItemAdded.AddDynamic(this, &UInventoryEquipmentMenuWidget::AddItem);
			InventoryComp->OnItemRemoved.AddDynamic(this, &UInventoryEquipmentMenuWidget::RemoveItem);
		}
	}

	InitializeGrid();

	return Result;
}

void UInventoryEquipmentMenuWidget::InitializeGrid()
{
	if (SlotWidgetClass == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryMenu::InitializeGrid - No SlotWidgetClass set in %s. Verify that SlotWidgetClass is correctly set to a valid UInventoryEquipmentSlotWidget blueprint subclass."), *GetName());
		return;
	}

	if (!InventoryGridPanelName.IsNone())
	{
		UGridPanel* InventoryGrid = Cast<UGridPanel>(WidgetTree->FindWidget(InventoryGridPanelName));
		if (InventoryGrid)
		{
			for (int SlotIndex = 0; SlotIndex < NumberOfInventorySlots; SlotIndex++)
			{
				FString SlotString = FString("InventorySlot");
				SlotString.AppendInt(SlotIndex);
				FName SlotName = FName(*SlotString);

				UInventoryEquipmentSlotWidget* InventorySlot = Cast<UInventoryEquipmentSlotWidget>(CreateWidget(GetOwningPlayer(), SlotWidgetClass, SlotName));
				if (InventorySlot)
				{
					InventorySlots.Add(InventorySlot);
					UGridSlot* GridSlot = InventoryGrid->AddChildToGrid(InventorySlot);
					int SlotRow = SlotIndex / NumberOfGridColumns;
					int SlotColumn = SlotIndex % NumberOfGridColumns;
					GridSlot->SetRow(SlotRow);
					GridSlot->SetColumn(SlotColumn);
					GridSlot->SetPadding(GridSlotPadding);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryMenu::InitializeGrid - No InventoryGrid widget found in %s. Verify that InventoryGridPanelName is correctly set."), *GetName());
			return;
		}
	}
}

void UInventoryEquipmentMenuWidget::AddItem(AItem* Item)
{
	// Place item in first available slot if any
	for (UInventoryEquipmentSlotWidget* Slot : InventorySlots)
	{
		if (!Slot->IsOccupied())
		{
			Slot->SetItem(Item);
		}
	}
}

void UInventoryEquipmentMenuWidget::RemoveItem(AItem* Item)
{
	// Search slots for desired item and remove if found
	for (UInventoryEquipmentSlotWidget* Slot : InventorySlots)
	{
		if (Slot->GetItem() == Item)
		{
			Slot->SetItem(nullptr);
		}
	}
}
