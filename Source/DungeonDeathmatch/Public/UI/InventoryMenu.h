// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMenu.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The number of inventory slots this menu should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	uint8 InventorySlots;

	/** The maximum number of columns the inventory grid should have. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	uint8 NumberOfGridColumns;
};
