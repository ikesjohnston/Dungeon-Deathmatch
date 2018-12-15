// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryEquipmentMenuWidget.generated.h"

class UInventoryEquipmentSlotWidget;

/**
 * UI widget representing the player inventory. 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryEquipmentMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The number of inventory slots this menu should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	uint8 NumberOfInventorySlots;

	/** The maximum number of columns the inventory grid should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	uint8 NumberOfGridColumns;
	
	/**
	 * The name of the GridPanel that is contained in this widget in the editor, which will hold the individual InventoryEquipmentSlots.
	 * Used to find the widget from the WidgetTree.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName InventoryGridPanelName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UInventoryEquipmentSlotWidget*> InventorySlots;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeGrid();
};
