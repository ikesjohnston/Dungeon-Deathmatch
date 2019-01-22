// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item.h"
#include "InventoryGridWidget.generated.h"

class UInventoryGridSlotWidget;
class UDraggableItemWidget;

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
	 * The name of the GridPanel that is contained in this widget in the editor, which will hold the individual Inventory UInventoryGridSlotWidgets.
	 * Used to find the widget from the WidgetTree.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName InventoryGridPanelName;

	/*
	 * The name of the CanvasPanel that is contained in this widget in the editor, which sits on top of the grid panel and holds drag and drop widgets for any items in the inventory.
	 * Used to find the widget from the WidgetTree.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName InventoryCanvasPanelName;

	/* The blueprint UInventoryGridSlotWidget subclass to populate the menu with */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInventoryGridSlotWidget> SlotWidgetClass;

	/* The array of UInventoryGridSlotWidgets that make up the inventory grid */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UInventoryGridSlotWidget*> InventorySlots;

	/* Mapping of items added to the inventory grid and their respective draggable widgets.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<AItem*, UDraggableItemWidget*> DraggableItemWidgets;

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
	 * Updates the number of cells in the inventory grid widget
	 *
	 * @param Rows The number of rows the grid should have
	 * @param Columns The number of columns the grid should have
	 */
	UFUNCTION()
	void UpdateInventoryGridSize(uint8 Rows, uint8 Columns);
};
