// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentGlobals.h"
#include "InventoryMenuWidget.generated.h"

class UInventoryGridWidget;
class UEquipmentSlotWidget;
class UInteractiveCharacterRenderWidget;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInventoryMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The inventory grid widget that displays the character inventory */
	UPROPERTY(meta = (BindWidget))
	UInventoryGridWidget* InventoryGrid;

	/** The character preview grid widget that displays the character model */
	UPROPERTY(meta = (BindWidget))
	UInteractiveCharacterRenderWidget* InteractiveCharacterRender;

	// ----------------------------------------- BEGIN EQUIPMENT WIDGET VARIABLES -----------------------------------------

	/** The equipment slot widget for head equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotHead;

	/** The equipment slot widget for neck equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotNeck;

	/** The equipment slot widget for shoulder equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotShoulders;

	/** The equipment slot widget for chest equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotChest;

	/** The equipment slot widget for hands equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotHands;

	/** The first equipment slot widget for finger equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFingerOne;

	/** The second equipment slot widget for finger equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFingerTwo;

	/** The equipment slot widget for waist equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotWaist;

	/** The equipment slot widget for legs equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLegs;

	/** The equipment slot widget for feet equipment */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFeet;

	/** The equipment slot widget for loadout one main hand weapons */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutOneWeaponMainHand;

	/** The equipment slot widget for loadout one off hand weapons */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutOneWeaponOffHand;

	/** The equipment slot widget for loadout two main hand weapons */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutTwoWeaponMainHand;

	/** The equipment slot widget for loadout two off hand weapons */
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutTwoWeaponOffHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, UEquipmentSlotWidget*> EquipmentSlots;

	// ----------------------------------------- END EQUIPMENT WIDGET VARIABLES -----------------------------------------

public:
	UInventoryMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
};
