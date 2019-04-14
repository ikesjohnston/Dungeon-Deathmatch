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
 * Widget that displays a character's inventory and equipment.
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UInteractiveCharacterRenderWidget* InteractiveCharacterRender;

	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotHead;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotNeck;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotShoulders;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotChest;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotHands;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFingerOne;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFingerTwo;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotWaist;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLegs;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotFeet;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutOneWeaponMainHand;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutOneWeaponOffHand;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutTwoWeaponMainHand;
	UPROPERTY(meta = (BindWidget))
	UEquipmentSlotWidget* EquipmentSlotLoadoutTwoWeaponOffHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, UEquipmentSlotWidget*> EquipmentSlots;

private:
	/** The time, in seconds, to wait before retrying binding on failure */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	float BindingRetryTime;

	/** Has the slot been bound to a player controller yet? */
	bool bIsSlotBound;

	/** Timer handle for retrying slot binding on failure */
	FTimerHandle BindSlotTimerHandle;

public:
	UInventoryMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

protected:
	/** Attempts to get the local player controller and bind the menu to it */
	void BindToController();

};
