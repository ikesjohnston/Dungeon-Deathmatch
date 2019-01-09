// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class AItem;

/**
 * Widget representing an item in an inventory. Processes drag and drop operations between equipment or other inventories.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

	// Only Inventory and Equipment menu widgets should be able to access this widget's data
	friend class UInventoryEquipmentMenuWidget;

protected:
	/** A reference to the item that this slot represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

	/** The number of items in this item stack, if one exists */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	uint8 StackNumber;

public:
	UFUNCTION(BlueprintCallable)
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	AItem* GetItem();

	UFUNCTION(BlueprintCallable)
	void SetItem(AItem* NewItem);
};
