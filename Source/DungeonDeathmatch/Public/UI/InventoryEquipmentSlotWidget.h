// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Equippable.h"
#include "InventoryEquipmentSlotWidget.generated.h"

class AItem;

/**
 * Widget representing an item in an inventory. Processes drag and drop operations between equipment or other inventories.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

// Only Inventory and Equipment menu widgets should be able to access this widget's data
friend class UInventoryEquipmentMenuWidget;

protected:
	/** A reference to the item that this slot represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

	/** Whether this slot is an Equipment slot or a standard Inventory slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bIsEquipmentSlot;

	/** The type of equipment this slot is for, if any. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bIsEquipmentSlot"))
	EEquipmentSlot EquipmentSlot;

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
