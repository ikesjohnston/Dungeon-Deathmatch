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
#include <WidgetLayoutLibrary.h>
#include <SlateBlueprintLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Reply.h>
#include "DungeonCharacter.h"

// Console command for debugging inventory grid widgets
static int32 DebugInventoryGrid = 0;
FAutoConsoleVariableRef CVARDebugInventoryGrid(
	TEXT("Dungeon.DebugInventoryGrid"),
	DebugInventoryGrid,
	TEXT("Debug inventory grid widget overlap calculations"),
	ECVF_Cheat);

UInventoryGridWidget::UInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::InitializeGrid - No InventoryGrid widget found in %s. Verify that InventoryGrid is correctly set."), *GetName());
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
			if (GridSlotIndex < InventorySlots.Num())
			{
				UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
				GridSlot->SetItem(Item);
			}
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
			DraggableWidget->InitializeDraggableItem(Item, false, OriginGridSlot);
			if (DraggableItemsCanvas)
			{
				DraggableItemsCanvas->AddChild(DraggableWidget);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::AddItem - No DraggableItemsCanvas widget found in %s. Verify that DraggableItemsCanvas is correctly set."), *GetName());
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
			if (GridSlotIndex < InventorySlots.Num())
			{
				UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
				GridSlot->SetItem(nullptr);
			}
		}
	}

	// Just remove the widget from the canvas and widget map, garbage collection should automatically delete it, not able to safely do so manually
	UDraggableItemWidget** WidgetPtr = DraggableItemWidgets.Find(Item);
	if (WidgetPtr)
	{
		UDraggableItemWidget* DraggableWidget = *WidgetPtr;
		DraggableWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (DraggableItemsCanvas)
		{
			DraggableItemsCanvas->RemoveChild(DraggableWidget);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::RemoveItem - No DraggableItemsCanvas widget found in %s. Verify that DraggableItemsCanvas is correctly set."), *GetName());
		}

		DraggableItemWidgets.Remove(Item);
	}
}

bool UInventoryGridWidget::ValidateGridSelection(AItem* Item)
{
	FInventoryGridPair ItemSize = Item->GetGridSize();
	uint8 ItemRowExtent = SelectionOrigin.Row + ItemSize.Row;
	uint8 ItemColumnExtent = SelectionOrigin.Column + ItemSize.Column;

	// Go through the selected grid slots once to determine if there is more than one item in the selection area
	AItem* SelectedItem = nullptr;
	bool SelectionValid = true;
	for (int GridRowIndex = SelectionOrigin.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
	{
		for (int GridColumIndex = SelectionOrigin.Column; GridColumIndex < ItemColumnExtent; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
			AItem* ItemInSlot = GridSlot->GetItem();
			if (ItemInSlot)
			{
				if (SelectedItem && SelectedItem != ItemInSlot)
				{
					SelectionValid = false;
					break;
				}
				else
				{
					SelectedItem = ItemInSlot;
				}
			}
		}
		if (!SelectionValid)
		{
			break;
		}
	}

	// Go back through the selected grid slots one more time and add the appropriate highlighting
	for (int GridRowIndex = SelectionOrigin.Row; GridRowIndex < ItemRowExtent; GridRowIndex++)
	{
		for (int GridColumIndex = SelectionOrigin.Column; GridColumIndex < ItemColumnExtent; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
			GridSlot->BeginItemOverlap(SelectionValid);
		}
	}

	bWereSlotsHighlightedLastFrame = true;

	if (SelectionValid)
	{
		UDraggableItemWidget** WidgetPtr = DraggableItemWidgets.Find(SelectedItem);
		if (WidgetPtr)
		{
			ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
			if (Controller)
			{
				Controller->SetSelectedItem(*WidgetPtr);
			}
		}
	}

	return SelectionValid;
}

void UInventoryGridWidget::ClearGridHighlights()
{
	// Go through the entire grid once and remove any active highlighting
	for (int GridRowIndex = 0; GridRowIndex < InventoryGridSize.Row; GridRowIndex++)
	{
		for (int GridColumIndex = 0; GridColumIndex < InventoryGridSize.Column; GridColumIndex++)
		{
			uint8 GridSlotIndex = (GridRowIndex * InventoryGridSize.Column) + GridColumIndex;
			UInventoryGridSlotWidget* GridSlot = InventorySlots[GridSlotIndex];
			GridSlot->EndItemOverlap();
		}
	}
	bWereSlotsHighlightedLastFrame = false;
}

void UInventoryGridWidget::UpdateInventoryGridSize(uint8 Rows, uint8 Columns)
{

}

void UInventoryGridWidget::ProcessItemDragAndDrop()
{
	if (bIsSelectionValid)
	{
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			ADungeonCharacter* Character = Cast<ADungeonCharacter>(Controller->GetPawn());
			if (Character)
			{
				UDraggableItemWidget* DraggedItemWidget = Controller->GetDraggedItem();
				UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
				if (DraggedItemWidget) {
					if (SelectedItemWidget)
					{
						Controller->StopDraggingItem(false);
						SelectedItemWidget->StartDragging();
						Character->Server_RequestRemoveItemFromInventory(SelectedItemWidget->GetItem());
						Character->Server_RequestAddItemToInventoryAtLocation(DraggedItemWidget->GetItem(), SelectionOrigin);
						Controller->SetSelectedItem(nullptr);
					}
					else
					{
						Character->Server_RequestAddItemToInventoryAtLocation(DraggedItemWidget->GetItem(), SelectionOrigin);
						Controller->StopDraggingItem(false);
						Controller->SetSelectedItem(nullptr);
					}
				}
				else if (SelectedItemWidget)
				{
					SelectedItemWidget->StartDragging();
					Character->Server_RequestRemoveItemFromInventory(SelectedItemWidget->GetItem());
				}
			}
		}
	}
}

void UInventoryGridWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (bWereSlotsHighlightedLastFrame)
	{
		ClearGridHighlights();
	}

	if (bIsHovering)
	{
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			UDraggableItemWidget* DraggedItemWidget = Controller->GetDraggedItem();
			if (DraggedItemWidget) {
				Controller->SetSelectedItem(nullptr);
				AItem* DraggedItem = DraggedItemWidget->GetItem();
				if (DraggedItem)
				{
					FVector2D MouseViewportPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

					FVector2D GridPixelPosition;
					FVector2D GridViewportPosition;
					USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), FVector2D(0, 0), GridPixelPosition, GridViewportPosition);

					FVector2D MouseOffset = MouseViewportPosition - GridViewportPosition;

					UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
					if (GameInstance)
					{
						// Calculate which grid slot is being hovered over and where the mouse is within that slot
						float GridSlotSize = GameInstance->GetInventoryGridSlotSize();

						int OverlappedSlotColumn = FMath::RoundToInt((MouseOffset.X / GridSlotSize) - 0.5f);
						int OverlappedSlotRow = FMath::RoundToInt((MouseOffset.Y / GridSlotSize) - 0.5f);

						float SlotOverlapPositionX = MouseOffset.X - (OverlappedSlotColumn * GridSlotSize);
						float SlotOverlapPositionY = MouseOffset.Y - (OverlappedSlotRow * GridSlotSize);

						int SlotIndex = (OverlappedSlotRow * InventoryGridSize.Column) + OverlappedSlotColumn;
						if (SlotIndex >= 0 && SlotIndex < InventorySlots.Num())
						{
							if (DebugInventoryGrid)
							{
								UInventoryGridSlotWidget* OverlappedSlotWidget = InventorySlots[SlotIndex];

								TArray<FStringFormatArg> FormatArgs;

								FormatArgs.Add(FStringFormatArg(OverlappedSlotColumn));
								FormatArgs.Add(FStringFormatArg(OverlappedSlotRow));
								FormatArgs.Add(FStringFormatArg(FMath::RoundToInt(SlotOverlapPositionX)));
								FormatArgs.Add(FStringFormatArg(FMath::RoundToInt(SlotOverlapPositionY)));
								FormatArgs.Add(OverlappedSlotWidget->GetName());

								FString DebugString = FString::Format(TEXT("Overlapping slot ({0}, {1}) at ({2}, {3}), which is {4}"), FormatArgs);

								UKismetSystemLibrary::PrintString(GetWorld(), DebugString, true, true, FLinearColor::Red, DeltaTime);
							}

							// Calculate the origin slot of where this item should be inserted, based on the item size and mouse cursor position
							FInventoryGridPair ItemSize = DraggedItem->GetGridSize();

							int HoveringOriginColumn = 0;
							int HoveringOriginRow = 0;

							if (ItemSize.Column <= InventoryGridSize.Column && ItemSize.Row <= InventoryGridSize.Row)
							{
								if (ItemSize.Column % 2 == 0)
								{
									HoveringOriginColumn = OverlappedSlotColumn - (ItemSize.Column / 2);
									if (SlotOverlapPositionX > (GridSlotSize / 2) && SlotOverlapPositionX <= GridSlotSize)
									{
										HoveringOriginColumn += 1;
									}
								}
								else
								{
									HoveringOriginColumn = OverlappedSlotColumn - ((ItemSize.Column - 1) / 2);
								}
								if (HoveringOriginColumn < 0)
								{
									HoveringOriginColumn = 0;
								}


								if (ItemSize.Row % 2 == 0)
								{
									HoveringOriginRow = OverlappedSlotRow - (ItemSize.Row / 2);
									if (SlotOverlapPositionY > (GridSlotSize / 2) && SlotOverlapPositionY <= GridSlotSize)
									{
										HoveringOriginRow += 1;
									}
								}
								else
								{
									HoveringOriginRow = OverlappedSlotRow - ((ItemSize.Row - 1) / 2);
								}
								if (HoveringOriginRow < 0)
								{
									HoveringOriginRow = 0;
								}

								SelectionOrigin.Column = HoveringOriginColumn;
								SelectionOrigin.Row = HoveringOriginRow;

								// Adjust the insertion origin location if the item won't fit at the current origin slot (for example, trying to insert at the bottom or far right end of the grid)
								int InsertionColumnExtent = SelectionOrigin.Column + ItemSize.Column;
								if (InsertionColumnExtent > InventoryGridSize.Column)
								{
									int ColumnOffset = InventoryGridSize.Column - InsertionColumnExtent;
									SelectionOrigin.Column += ColumnOffset;
								}

								int InsertionRowExtent = SelectionOrigin.Row + ItemSize.Row;
								if (InsertionRowExtent > InventoryGridSize.Row)
								{
									int RowOffset = InventoryGridSize.Row - InsertionRowExtent;
									SelectionOrigin.Row += RowOffset;
								}

								SlotIndex = (SelectionOrigin.Row * InventoryGridSize.Column) + SelectionOrigin.Column;
								if (SlotIndex >= 0 && SlotIndex < InventorySlots.Num())
								{
									if (DebugInventoryGrid)
									{
										UInventoryGridSlotWidget* OverlappedSlotWidget = InventorySlots[SlotIndex];

										TArray<FStringFormatArg> FormatArgs;

										FormatArgs.Add(FStringFormatArg(SelectionOrigin.Column));
										FormatArgs.Add(FStringFormatArg(SelectionOrigin.Row));
										FormatArgs.Add(OverlappedSlotWidget->GetName());

										FString DebugString = FString::Format(TEXT("Item would be inserted at ({0}, {1}), which is slot {2}"), FormatArgs);

										UKismetSystemLibrary::PrintString(GetWorld(), DebugString, true, true, FLinearColor::Red, DeltaTime);
									}

									bIsSelectionValid = ValidateGridSelection(DraggedItem);
								}
							}
						}
					}
				}
			}
		}
	}

	Super::NativeTick(MyGeometry, DeltaTime);
}

void UInventoryGridWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsHovering = true;
}

void UInventoryGridWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bIsHovering = false;
}

FReply UInventoryGridWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		ProcessItemDragAndDrop();
	}

	return FReply::Handled();
}

FReply UInventoryGridWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			UDraggableItemWidget* SelectedItem = Controller->GetSelectedItem();
			UDraggableItemWidget* DraggedItem = Controller->GetDraggedItem();
			if (DraggedItem)
			{
				ProcessItemDragAndDrop();
			}
			else if (SelectedItem && SelectedItem->IsReadyForDrag())
			{
				SelectedItem->StartDragging();
			}
		}
	}

	return FReply::Handled();
}
