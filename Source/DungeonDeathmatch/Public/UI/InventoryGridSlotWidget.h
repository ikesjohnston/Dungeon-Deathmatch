// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlotWidget.generated.h"

class UImage;
class AItem;

/**
 * Widget representing a single slot in an inventory grid
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryGridSlotWidget : public UUserWidget
{
	GENERATED_BODY()

	// Only Inventory and Equipment menu widgets should be able to access this widget's data
	friend class UInventoryEquipmentMenuWidget;

protected:
	/** The image widget for the slot background */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotBackground;

	/** The image widget for the slot highlight, used when the slot is being overlapped by a drag and drop operation */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotHighlight;

	/** The color to tint the slot background by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor DefaultBackgroundColor;

	/** The color to tint the slot background during a valid overlap event */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor ValidOverlapHighlightColor;

	/** The color to tint the slot background during an invalid overlap event */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor InvalidOverlapHighlightColor;

	/** A reference to the item that occupies this slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

public:
	UInventoryGridSlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/** Get the item that is occupying this slot */
	UFUNCTION(BlueprintCallable)
	AItem* GetItem();

	/** Set the item that is occupying this slot */
	UFUNCTION(BlueprintCallable)
	void SetItem(AItem* NewItem);

	/** Event for when a draggable item begins overlapping this slot */
	UFUNCTION(BlueprintCallable)
	void BeginItemOverlap(bool IsOverlapValid);

	/** Event for when a draggable item stops overlapping this slot */
	UFUNCTION(BlueprintCallable)
	void EndItemOverlap();

};
