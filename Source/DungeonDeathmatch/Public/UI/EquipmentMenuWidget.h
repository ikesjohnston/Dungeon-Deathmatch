// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Equippable.h"
#include "EquipmentMenuWidget.generated.h"

class UEquipmentSlotWidget;
class UDraggableItemWidget;

UCLASS()
class DUNGEONDEATHMATCH_API UEquipmentMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The blueprint UEquipmentSlotWidget subclass to populate the menu with for each equipment slot*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UEquipmentSlotWidget> SlotWidgetClass;

	/** Mapping of equipment slots and thier respective slot widgets.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<EEquipmentSlot, UEquipmentSlotWidget*> EquipmentSlots;

	/** Mapping of equipped items and their respective draggable widgets.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<AEquippable*, UDraggableItemWidget*> DraggableItemWidgets;

public:
	UEquipmentMenuWidget(const FObjectInitializer& ObjectInitializer);
};
