// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryGridSlotWidget.generated.h"

class UImage;
class AItem;

/**
 * Widget representing a single slot in an inventory grid.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryGridSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* SlotBackground;

	/** The image widget for the slot highlight, used when the slot is being overlapped by a drag and drop operation */
	UPROPERTY(meta = (BindWidget))
	UImage* SlotHighlight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor DefaultBackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor ValidOverlapHighlightColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	FLinearColor InvalidOverlapHighlightColor;

	/** A reference to the item that occupies this slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	AItem* GetItem() { return Item; };

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItem(AItem* NewItem);

	/** Event for when a draggable item begins overlapping this slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void BeginItemOverlap(bool IsOverlapValid);

	/** Event for when a draggable item stops overlapping this slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EndItemOverlap();

private:
	bool ValidateWidgets();
};
