// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Equippable.h"
#include "InventoryEquipmentSlot.generated.h"

class AItem;

/**
 * Widget representing an item in an inventory. Processes drag and drop operations between equipment or other inventories.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()

// Only Inventory and Equipment menu widgets should be able to access this widget's data
friend class UInventoryMenu;
friend class UEquipmentMenu;

protected:
	/** A reference to the item that this slot represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* Item;

	/** Whether this slot is an Equipment slot or a standard Inventory slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bIsEquipmentSlot;

	/** The type of equipment this slot is for, if any. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bIsEquipmentSlot"))
	EEquipmentSlot EquipmentSlotType;

	/** The number of items in this item stack, if one exists */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	uint8 StackNumber;
};
