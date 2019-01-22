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

	/** The color to tint the slot background by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FLinearColor DefaultBackgroundColor;

	/** A reference to the item that occupies this slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

public:
	UInventoryGridSlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/** Get the item that is occupying this slot*/
	UFUNCTION(BlueprintCallable)
	AItem* GetItem();

	/** Set the item that is occupying this slot*/
	UFUNCTION(BlueprintCallable)
	void SetItem(AItem* NewItem);
};
