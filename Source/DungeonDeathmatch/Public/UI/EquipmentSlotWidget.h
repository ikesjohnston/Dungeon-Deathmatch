// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "EquipmentGlobals.h"
#include "Item.h"
#include "EquipmentSlotWidget.generated.h"

class UImage;
class UCanvasPanel;

class UDraggableItemWidget;
class UEquipmentComponent;
class UInventoryComponent;

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

	/** The canvas widget that draggable equipment widgets will be added and removed from */
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DraggableEquipmentCanvas;

	/** The draggable item widget for whatever item is equipped in the slot, if any*/
	UPROPERTY()
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

	/* Sound to play when a drag operation starts */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	USoundCue* BeginDragSound;

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

	/** The color to tint the default item image for off hand weapon slots when a two handed weapon is equipped in its respective slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor LockedSlotItemColor;

private:
	/** Has the slot been bound to a player controller yet? */
	bool bIsBoundToController;

	/** Timer handle for retrying slot binding on failure */
	FTimerHandle BindToControllerTimerHandle;

	UEquipmentComponent* SourceEquipmentComponent;
	UInventoryComponent* SourceInventoryComponent;

public:
	UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

protected:
	/** Attempts to get the local player controller and bind the slot to it */
	void BindToController();

	/** Attempts to bind the slot to a particular source's EquipmentComponent*/
	void BindToSource(AActor* Source);

	/** Resets the default item image to the global default for the slot type */
	void ResetDefaultImage();

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

	/**
	 * Event for when a new item is equipped to the character that owns this widget
	 *
	 * @param Equippable The item that was equipped
	 * @param EquipmentSlot The equipment slot the item went into
	 */
	UFUNCTION()
	void OnItemEquipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/**
	 * Event for when an item is unequipped from the character that owns this widget
	 *
	 * @param Equippable The item that was unequipped
	 * @param EquipmentSlot The equipment slot the item was removed from
	 */
	UFUNCTION()
	void OnItemUnequipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
};
