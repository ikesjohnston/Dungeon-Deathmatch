// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGlobals.h"
#include "InventoryGridWidget.generated.h"

class UInventoryGridSlotWidget;
class UDraggableItemWidget;
class UGridPanel;
class UCanvasPanel;
class AItem;

/**
 * UI widget representing the player inventory.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/* The number of rows and columns that make up the inventory grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FInventoryGridPair InventoryGridSize;

	/*
	 * The GridPanel widget for the inventory grid, which will hold the individual Inventory UInventoryGridSlotWidgets.
	 */
	UPROPERTY(EditAnywhere, Category = "Widgets", meta = (BindWidget))
	UGridPanel* InventoryGrid;

	/*
	 * The CanvasPanel which sits on top of the grid panel and holds drag and drop widgets for any items in the inventory.
	 */
	UPROPERTY(EditAnywhere, Category = "Widgets", meta = (BindWidget))
	UCanvasPanel* DraggableItemsCanvas;

	/* The blueprint UInventoryGridSlotWidget subclass to populate the menu with */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInventoryGridSlotWidget> SlotWidgetClass;

	/* The array of UInventoryGridSlotWidgets that make up the inventory grid */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UInventoryGridSlotWidget*> InventorySlots;

	/* Mapping of items added to the inventory grid and their respective draggable widgets.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<AItem*, UDraggableItemWidget*> DraggableItemWidgets;

	/* Sound to play when a drag operation starts */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	USoundCue* BeginDragSound;

	/* Is the mouse hovering over this widget? */
	bool bIsHovering;

	/** Whether the current grid selection is a valid location for any currently dragged item */
	bool bIsSelectionValid;

	/** The upper left most grid slot coordinates where a dragged item will be placed if requested */
	FInventoryGridPair SelectionOrigin;

private:
	/** Used to only clear highlights when they are active */
	bool bWereSlotsHighlightedLastFrame;

public:
	UInventoryGridWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/**
	 * Initializes individual grid slot widgets and adds them to the inventory grid
	 */
	UFUNCTION(BlueprintCallable)
	void InitializeGrid();

protected:
	/**
	 * Adds an item to the inventory grid widget at the specified location
	 *
	 * @param Item The item to add to the grid
	 * @param OriginGridSlot The coordinates of the upper left most cell where the item is positioned
	 */
	UFUNCTION()
	void AddItem(AItem* Item, FInventoryGridPair OriginGridSlot);

	/**
	 * Removes an item from the inventory grid widget at the specified location
	 *
	 * @param Item The item to remove from the grid
	 * @param OriginGridSlot The coordinates of the upper left most cell where the item is positioned
	 */
	UFUNCTION()
	void RemoveItem(AItem* Item, FInventoryGridPair OriginGridSlot);

	/**
	 * Highlights grid slots underneath the currently dragged item, signaling where the item will be placed and if it is a valid location.
	 *
	 * @param Item The item that is being dragged over the grid
	 */
	bool ValidateGridSelection(AItem* Item);

	/** Removes highlights from all grid slots */
	void ClearGridHighlights();

	/**
	 * Updates the number of cells in the inventory grid widget
	 *
	 * @param Rows The number of rows the grid should have
	 * @param Columns The number of columns the grid should have
	 */
	UFUNCTION()
	void UpdateInventoryGridSize(uint8 Rows, uint8 Columns);

	/** Event for when a draggable item is dropped on top of the grid. Processes the validity of the drop and makes any necessary additions or replacements to the inventory. */
	void ProcessItemDragAndDrop();

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
};
