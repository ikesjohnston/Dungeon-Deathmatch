// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentEnums.h"
#include "Item.h"
#include "EquipmentSlotWidget.generated.h"

class UImage;
class UDraggableItemWidget;

/**
 * Widget for displaying an equipment slot and any item currently in that slot
 */
UCLASS()
class DUNGEONDEATHMATCH_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The image widget for the slot background */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotBackground;

	/** The image widget for the slot highlight */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotHighlight;

	/** The border image widget for the slot */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotBorder;

	/** The image widget for displaying a default item when nothing is equipped, to signify what goes in the slot */
	UPROPERTY(meta = (BindWidget))
	UImage* DefaultEquipmentImage;

	/** The draggable item widget for whatever item is equipped in the slot, if any*/
	UPROPERTY(meta = (BindWidget))
	UDraggableItemWidget* EquippedItemWidget;

	/** The type of equipment that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	EEquipmentSlot SlotType;

	/** The item textures to use for slots when nothing is equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, UTexture2D*> EquipmentSlotTextures;

	/** The grid sizes of each slot type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, FInventoryGridPair> EquipmentSlotSizes;

	/** The image grid sizes each slot type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, FInventoryGridPair> EquipmentTextureSizes;

	/** The time, in seconds, to wait before retrying binding on failure */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	float BindingRetryTime;

	/** Can this slot accept the currently dragged item? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bCanFitDraggedItem;

	/** The size of the slot border, in pixels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	float BorderSize;

	/** The color to tint the slot border by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor DefaultBorderColor;

	/** The color to tint the slot background by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor DefaultBackgroundColor;

	/** The color to tint the slot border when the slot can accept an item being dragged, or to tint the background when mousing over the slot while not dragging anything */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor HighlightColor;

	/** The color to tint the slot background when the slot can accept an item being dragged over the widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor ValidOverlapHighlightColor;

	/** The color to tint the slot background when the slot can not accept an item being dragged over the widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor InvalidOverlapHighlightColor;

private:
	/** Has the slot been bound to a player controller yet? */
	bool bIsSlotBound;

	/** Timer handle for retrying slot binding on failure */
	FTimerHandle BindSlotTimerHandle;

public:
	UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

protected:
	/** Attempts to get the local player controller and bind the slot to it */
	void BindToController();

	/** Updates the displayed item image for the slot and disables the draggable item widget if nothing is equipped */
	void UpdateEquipment();

	/** Can this slot accept the currently dragged item? */
	bool GetCanFitDraggedItem();

	/** Event for when a draggable item is dropped on top of the slot. Processes the validity of the drop and makes any necessary additions or replacements to equipment. */
	void ProcessItemDragAndDrop();

	/** Event called when the player controller starts dragging an item, used to highlight the slot if the item can go in it */
	UFUNCTION()
	void OnBeginItemDrag(AItem* Item);

	/** Event called when the player controller stops dragging an item, used to remove any active slot highlighting */
	UFUNCTION()
	void OnEndItemDrag(AItem* Item);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
