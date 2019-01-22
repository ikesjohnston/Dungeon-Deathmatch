// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryGridWidget.h"
#include "InventoryGridSlotWidget.h"
#include <GridPanel.h>
#include <UserWidget.h>
#include <GridSlot.h>
#include "InventoryComponent.h"
#include "DungeonPlayerController.h"
#include "DungeonGameInstance.h"
#include <CanvasPanel.h>
#include <WidgetTree.h>
#include "DraggableItemWidget.h"

UInventoryGridWidget::UInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridPanelName = "Grid_Inventory";
	InventoryCanvasPanelName = "Canvas_Inventory";

	InventoryGridSize = FInventoryGridPair(6, 5);

	InitializeGrid();
}

bool UInventoryGridWidget::Initialize()
{
	bool Result = Super::Initialize();
	// Get the player's inventory component and bind item add and remove events
	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(GetOwningPlayerPawn());
	if (OwningCharacter)
	{
		UInventoryComponent* InventoryComp = OwningCharacter->GetInventoryComponent();
		if (InventoryComp)
		{
			InventoryGridSize = InventoryComp->GetInventoryGridSize();
			InventoryComp->OnItemAdded.AddDynamic(this, &UInventoryGridWidget::AddItem);
			InventoryComp->OnItemRemoved.AddDynamic(this, &UInventoryGridWidget::RemoveItem);
			InventoryComp->OnInventorySizeChanged.AddDynamic(this, &UInventoryGridWidget::UpdateInventoryGridSize);
		}
	}

	InitializeGrid();

	return Result;
}

void UInventoryGridWidget::InitializeGrid()
{
	if (SlotWidgetClass == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::InitializeGrid - No SlotWidgetClass set in %s. Verify that SlotWidgetClass is correctly set to a valid UInventoryGridSlotWidget blueprint subclass."), *GetName());
		return;
	}

	if (!InventoryGridPanelName.IsNone())
	{
		UGridPanel* InventoryGrid = Cast<UGridPanel>(WidgetTree->FindWidget(InventoryGridPanelName));
		if (InventoryGrid)
		{
			for (int GridRowIndex = 0; GridRowIndex < InventoryGridSize.Row; GridRowIndex++)
			{
				for (int GridColumIndex = 0; GridColumIndex < InventoryGridSize.Column; GridColumIndex++)
				{
					uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
					FString GridSlotString = FString("InventorySlot");
					GridSlotString.AppendInt(GridSlotIndex);
					FName GridSlotName = FName(*GridSlotString);

					UInventoryGridSlotWidget* InventorySlot = Cast<UInventoryGridSlotWidget>(CreateWidget(GetOwningPlayer(), SlotWidgetClass, GridSlotName));
					if (InventorySlot)
					{
						InventorySlots.Add(InventorySlot);
						UGridSlot* GridSlot = InventoryGrid->AddChildToGrid(InventorySlot);
						GridSlot->SetRow(GridRowIndex);
						GridSlot->SetColumn(GridColumIndex);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::InitializeGrid - No Inventory Grid widget found in %s. Verify that InventoryGridPanelName is correctly set."), *GetName());
		}
	}
}

void UInventoryGridWidget::AddItem(AItem* Item, FInventoryGridPair OriginGridSlot)
{
	// Update the grid widget
	FInventoryGridPair ItemSize = Item->GetGridSize();
	uint8 ItemRowExtent = OriginGridSlot.Row + ItemSize.Row;
	uint8 ItemColumnExtent = OriginGridSlot.Column + ItemSize.Column;

	for (int GridRowIndex = OriginGridSlot.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
	{
		for (int GridColumIndex = OriginGridSlot.Column; GridColumIndex < ItemColumnExtent; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
			GridSlot->SetItem(Item);
		}
	}

	// Add the drag and drop widget on top of the inventory grid
	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		FString DragAndDropString = FString("Drag&Drop_");
		DragAndDropString.Append(Item->GetItemName().ToString());
		FName DragAndDropName = FName(*DragAndDropString);

		UDraggableItemWidget* DraggableWidget = Cast<UDraggableItemWidget>(CreateWidget(GetOwningPlayer(), GameInstance->GetDragAndDropItemWidgetClass(), DragAndDropName));
		if (DraggableWidget)
		{
			DraggableWidget->SetItem(Item);
			DraggableWidget->SetItemGridLocation(OriginGridSlot);
			UCanvasPanel* InventoryCanvas = Cast<UCanvasPanel>(WidgetTree->FindWidget(InventoryCanvasPanelName));
			if (InventoryCanvas)
			{
				InventoryCanvas->AddChild(DraggableWidget);
			}
			DraggableItemWidgets.Add(TTuple<AItem*, UDraggableItemWidget*>(Item, DraggableWidget));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::AddItem - Failed to get game instance."));
	}
}

void UInventoryGridWidget::RemoveItem(AItem* Item, FInventoryGridPair OriginGridSlot)
{
	// Update the grid widget
	FInventoryGridPair ItemSize = Item->GetGridSize();
	uint8 ItemRowExtent = OriginGridSlot.Row + ItemSize.Row;
	uint8 ItemColumnExtent = OriginGridSlot.Column + ItemSize.Column;

	for (int GridRowIndex = OriginGridSlot.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
	{
		for (int GridColumIndex = OriginGridSlot.Column; GridColumIndex < ItemColumnExtent; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
			GridSlot->SetItem(nullptr);
		}
	}

	UDraggableItemWidget** WidgetPtr = DraggableItemWidgets.Find(Item);
	if (WidgetPtr)
	{
		UDraggableItemWidget* DraggableWidget = *WidgetPtr;
		UCanvasPanel* InventoryCanvas = Cast<UCanvasPanel>(WidgetTree->FindWidget(InventoryCanvasPanelName));
		if (InventoryCanvas)
		{
			InventoryCanvas->RemoveChild(DraggableWidget);
		}
	}
}

void UInventoryGridWidget::UpdateInventoryGridSize(uint8 Rows, uint8 Columns)
{

}
