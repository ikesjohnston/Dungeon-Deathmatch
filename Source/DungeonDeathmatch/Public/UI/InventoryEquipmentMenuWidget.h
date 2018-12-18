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
	/* The number of inventory slots this menu should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	uint8 NumberOfInventorySlots;

	/* The maximum number of columns the inventory grid should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	uint8 NumberOfGridColumns;
	
	/* The padding, in pixels, around each grid slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0))
	float GridSlotPadding;

	/*
	 * The name of the GridPanel that is contained in this widget in the editor, which will hold the individual UInventoryEquipmentSlotWidgets.
	 * Used to find the widget from the WidgetTree.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName InventoryGridPanelName;

	/* The blueprint UInventoryEquipmentSlotWidget subclass to populate the menu with */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInventoryEquipmentSlotWidget> SlotWidgetClass;

	/* The array of UInventoryEquipmentSlotWidgets that make up the inventory grid */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UInventoryEquipmentSlotWidget*> InventorySlots;

public:
	UInventoryEquipmentMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

protected:
	void InitializeGrid();

	void AddItem(AItem* Item);

	void RemoveItem(AItem* Item);
};
