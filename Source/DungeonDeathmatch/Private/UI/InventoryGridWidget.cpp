// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryGridWidget.h"
#include "InventoryGridSlotWidget.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "DungeonPlayerController.h"
#include "DungeonGameInstance.h"
#include "DraggableItemWidget.h"
#include "DungeonHUD.h"

#include <GridPanel.h>
#include <UserWidget.h>
#include <GridSlot.h>
#include <CanvasPanel.h>
#include <WidgetTree.h>
#include <WidgetLayoutLibrary.h>
#include <SlateBlueprintLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Reply.h>
#include <WidgetBlueprintLibrary.h>

UInventoryGridWidget::UInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridSize = FInventoryGridPair(6, 5);
	InitializeGrid();
}

bool UInventoryGridWidget::Initialize()
{
	bool Result = Super::Initialize();
	if (!ValidateWidgets())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::Initialize - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());
		Result = false;
	}
	else
	{
		InitializeGrid();
	}
	return Result;
}

void UInventoryGridWidget::BindToSource(AActor* Source)
{
	if (!ValidateWidgets()) return;
	
	InventoryGrid->ClearChildren();

	if (SourceInventoryComponent)
	{
		InventoryGridSize = SourceInventoryComponent->GetInventoryGridSize();
		SourceInventoryComponent->OnItemAdded.RemoveDynamic(this, &UInventoryGridWidget::AddItem);
		SourceInventoryComponent->OnItemRemoved.RemoveDynamic(this, &UInventoryGridWidget::RemoveItem);
	}

	SourceInventoryComponent = Cast<UInventoryComponent>(Source->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (SourceInventoryComponent)
	{
		InventoryGridSize = SourceInventoryComponent->GetInventoryGridSize();
		SourceInventoryComponent->OnItemAdded.AddDynamic(this, &UInventoryGridWidget::AddItem);
		SourceInventoryComponent->OnItemRemoved.AddDynamic(this, &UInventoryGridWidget::RemoveItem);
	}

	SourceEquipmentComponent = Cast<UEquipmentComponent>(Source->GetComponentByClass(UEquipmentComponent::StaticClass()));

	InitializeGrid();

	if (SourceInventoryComponent)
	{
		TArray<AItem*> AddedSourceItems;
		TArray<FInventoryGridSlot> SourceInventoryGrid = SourceInventoryComponent->GetInventoryGrid();
		for (FInventoryGridSlot& GridSlot : SourceInventoryGrid)
		{
			AItem* SlotItem = GridSlot.Item;
			if (SlotItem && !AddedSourceItems.Contains(SlotItem))
			{
				AddItem(SlotItem, GridSlot.ItemOriginGridLocation);
				AddedSourceItems.Add(SlotItem);
			}
		}
	}
}

void UInventoryGridWidget::InitializeGrid()
{
	if (!ValidateWidgets()) return;

	if (SlotWidgetClass == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridWidget::InitializeGrid - No SlotWidgetClass set in %s. Verify that SlotWidgetClass is correctly set to a valid UInventoryGridSlotWidget blueprint subclass."), *GetName());
		return;
	}

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

void UInventoryGridWidget::AddItem(AItem* Item, FInventoryGridPair OriginGridSlot)
{
	if (!ValidateWidgets()) return;

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
		TSubclassOf<UDraggableItemWidget> DragAndDropItemWidgetClass = GameInstance->GetDragAndDropItemWidgetClass();
		if (DragAndDropItemWidgetClass == NULL) return;

		FString DragAndDropString = FString("Drag&Drop_");
		DragAndDropString.Append(Item->GetItemName().ToString());
		FName DragAndDropName = FName(*DragAndDropString);

		UDraggableItemWidget* DraggableWidget = Cast<UDraggableItemWidget>(CreateWidget(GetOwningPlayer(), DragAndDropItemWidgetClass, DragAndDropName));
		if (DraggableWidget)
		{
			DraggableWidget->InitializeDraggableItem(Item, OriginGridSlot);
			DraggableItemsCanvas->AddChild(DraggableWidget);
			DraggableWidget->SetVisibility(ESlateVisibility::Visible);
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
	if (!ValidateWidgets()) return;

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

	// Just remove the widget from the canvas and widget map, garbage collection should automatically delete it
	UDraggableItemWidget** WidgetPtr = DraggableItemWidgets.Find(Item);
	if (WidgetPtr)
	{
		UDraggableItemWidget* DraggableWidget = *WidgetPtr;
		DraggableItemsCanvas->RemoveChild(DraggableWidget);
		DraggableWidget->SetVisibility(ESlateVisibility::Collapsed);
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

void UInventoryGridWidget::ProcessItemDragAndDrop()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller && SourceInventoryComponent)
	{
		UDraggableItemWidget* DraggedItemWidget = Controller->GetDraggedItem();
		UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
		UDraggableItemWidget* ClickedItemWidget = Controller->GetClickedItem();
		if (DraggedItemWidget && bIsSelectionValid) {
			if (SelectedItemWidget)
			{
				// Selecting a replacement item to drag
				Controller->StopDraggingItem(false);
				SelectedItemWidget->StartDragging();
				AItem* SelectedItem = SelectedItemWidget->GetItem();
				if (SelectedItem)
				{
					SourceInventoryComponent->ServerRequestRemoveItemFromInventory(SelectedItem);
				}
				AItem* DraggedItem = DraggedItemWidget->GetItem();
				if (DraggedItem)
				{
					SourceInventoryComponent->ServerRequestAddItemToInventoryAtLocation(DraggedItem, SelectionOrigin);
					UGameplayStatics::PlaySound2D(GetWorld(), DraggedItem->GetInteractionSound());
				}
				Controller->SetSelectedItem(nullptr);
			}
			else
			{
				// Trying to drop the item at the current location
				AItem* DraggedItem = DraggedItemWidget->GetItem();
				if (DraggedItem)
				{
					SourceInventoryComponent->ServerRequestAddItemToInventoryAtLocation(DraggedItem, SelectionOrigin);
					UGameplayStatics::PlaySound2D(GetWorld(), DraggedItem->GetInteractionSound());
				}
				Controller->StopDraggingItem(false);
				Controller->SetSelectedItem(nullptr);
			}
		}
		else if (ClickedItemWidget)
		{
			// Selecting a new item to drag
			ClickedItemWidget->StartDragging();
			SourceInventoryComponent->ServerRequestRemoveItemFromInventory(ClickedItemWidget->GetItem());
			UGameplayStatics::PlaySound2D(GetWorld(), BeginDragSound);
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
		UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (GameInstance && Controller)
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

					// Calculate which grid slot is being hovered over and where the mouse is within that slot
					float GridSlotSize = GameInstance->GetInventoryGridSlotSize();

					int OverlappedSlotColumn = FMath::RoundToInt((MouseOffset.X / GridSlotSize) - 0.5f);
					int OverlappedSlotRow = FMath::RoundToInt((MouseOffset.Y / GridSlotSize) - 0.5f);

					float SlotOverlapPositionX = MouseOffset.X - (OverlappedSlotColumn * GridSlotSize);
					float SlotOverlapPositionY = MouseOffset.Y - (OverlappedSlotRow * GridSlotSize);

					int SlotIndex = (OverlappedSlotRow * InventoryGridSize.Column) + OverlappedSlotColumn;
					if (SlotIndex >= 0 && SlotIndex < InventorySlots.Num())
					{
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
								bIsSelectionValid = ValidateGridSelection(DraggedItem);
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

FReply UInventoryGridWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller && SourceInventoryComponent)
	{
		UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
		if (SelectedItemWidget)
		{
			if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				return ClickSelectedItem(InMouseEvent, Controller, SelectedItemWidget);
			}
			else if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
			{
				DropSelectedItem(Controller, SelectedItemWidget);
			}
			else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
			{
				UseSelectedItem(Controller, SelectedItemWidget);
			}
		}
	}
	return FReply::Handled();
}

FReply UInventoryGridWidget::ClickSelectedItem(const FPointerEvent& InMouseEvent, ADungeonPlayerController* Controller, UDraggableItemWidget* SelectedItemWidget)
{
	Controller->SetClickedItem(SelectedItemWidget);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UInventoryGridWidget::UseSelectedItem(ADungeonPlayerController* Controller, UDraggableItemWidget* SelectedItemWidget)
{
	// If the item is an equippable, try to equip it
	AEquippable* Equippable = Cast<AEquippable>(SelectedItemWidget->GetItem());
	if (Equippable)
	{
		SourceInventoryComponent->ServerRequestRemoveItemFromInventory(Equippable);
		if (SourceEquipmentComponent)
		{
			SourceEquipmentComponent->ServerEquipItem(Equippable, true);
		}
		UGameplayStatics::PlaySound2D(GetWorld(), Equippable->GetInteractionSound());
		Controller->SetSelectedItem(nullptr);
		ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
		if (HUD)
		{
			HUD->HideTooltip();
		}
	}
}

void UInventoryGridWidget::DropSelectedItem(ADungeonPlayerController* Controller, UDraggableItemWidget* SelectedItemWidget)
{
	AItem* ItemToDrop = SelectedItemWidget->GetItem();
	if (ItemToDrop)
	{
		SourceInventoryComponent->ServerRequestDropItem(ItemToDrop);
		UGameplayStatics::PlaySound2D(GetWorld(), ItemToDrop->GetInteractionSound());
		Controller->SetSelectedItem(nullptr);
		ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
		if (HUD)
		{
			HUD->HideTooltip();
		}
	}
}

FReply UInventoryGridWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			ProcessItemDragAndDrop();
			Controller->SetClickedItem(nullptr);
		}
	}

	return FReply::Handled();
}

void UInventoryGridWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		UDraggableItemWidget* ClickedItem = Controller->GetClickedItem();
		UDraggableItemWidget* DraggedItem = Controller->GetDraggedItem();
		if (ClickedItem && !DraggedItem)
		{
			Controller->SetSelectedItem(nullptr);
			ProcessItemDragAndDrop();
			Controller->SetClickedItem(nullptr);
		}
	}
}

bool UInventoryGridWidget::ValidateWidgets()
{
	bool Result = false;
	if (InventoryGrid && DraggableItemsCanvas)
	{
		Result = true;
	}
	return Result;
}

