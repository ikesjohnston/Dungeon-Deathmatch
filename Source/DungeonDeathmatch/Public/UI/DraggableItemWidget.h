// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item.h"
#include "DraggableItemWidget.generated.h"

class UCanvasPanel;
class UImage;
class UButton;

/**
 * A widget for displaying items in an inventory grid, used to initiate drag and drop operations
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDraggableItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The canvas widget for the draggable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UCanvasPanel* ItemCanvas;

	/** The item image widget for the draggable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UImage* ItemImage;

	/** The button widget for selecting the draggable item, currently just used for highlighting on hover */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UButton* ItemSelectButton;

	/** A reference to the item that this drag and drop widget represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	AItem* Item;

	/** The grid location of the item if in an inventory*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FInventoryGridPair GridLocation;

public:
	UDraggableItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/** Sets the item associated with this widget and adjusts the widget size and location to match its location context */
	void InitializeDraggableItem(AItem* DraggableItem, FInventoryGridPair InventoryGridLocation = FInventoryGridPair());

	/** Initializes the widget for a specifically sized equipment slot */
	void InitializeDraggableEquipment(AItem* DraggableItem, FInventoryGridPair SlotSize);

	/**  Gets the item associated with this widget */
	AItem* GetItem();

	/** Tells the local player controller to start dragging this widget's item. Does NOT remove the item from any inventory or equipment components or widgets. */
	void StartDragging();

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
