// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item.h"
#include "DraggableItemWidget.generated.h"

class UCanvasPanel;
class UImage;
class UBorder;
class UItemTooltipWidget;
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

	/** The tooltip widget for the draggable item, is displayed when hovering the mouse over the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UItemTooltipWidget* ItemTooltip;

	/** The button widget for selecting the draggable item, starts a drag and drop operation when clicked */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UButton* ItemSelectButton;

	/** A reference to the item that this drag and drop widget represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	AItem* Item;

	/** The grid location of the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FInventoryGridPair GridLocation;


public:
	UDraggableItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/**  Gets the item associated with this widget */
	AItem* GetItem();

	/** Sets the item associated with this widget and adjusts the background colors and widget location to match its grid location */
	UFUNCTION()
	void SetItem(AItem* NewItem);

	/** Sets the grid location for the item represented by this widget */
	UFUNCTION()
	void SetItemGridLocation(FInventoryGridPair NewGridLocation);

protected:
	/** Attempts to get the local player controller and bind the widget to it */
	void BindToController();

	/** Event for when the mouse starts hovering over the select item button */
	UFUNCTION()
	void OnItemSelectButtonHovered();

	/** Event for when the mouse stops hovering over the select item button */
	UFUNCTION()
	void OnItemSelectButtonUnhovered();

	/** Event for when the select item button is pressed */
	UFUNCTION()
	void OnItemSelectButtonPressed();

	/** Event for when the select item button is released */
	UFUNCTION()
	void OnItemSelectButtonReleased();

	/** Event called when the player controller starts dragging an item, used to darken the item image if it is the item being dragged */
	UFUNCTION()
	void OnBeginItemDrag(AItem* DraggedItem);

	/** Event called when the player controller stops dragging an item, used restore the item tint if it was the item no longer being dragged */
	UFUNCTION()
	void OnEndItemDrag(AItem* DraggedItem);
};
