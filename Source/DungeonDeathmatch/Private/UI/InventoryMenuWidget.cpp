// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryMenuWidget.h"

UInventoryMenuWidget::UInventoryMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UInventoryMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	// Map equipment slots
	EquipmentSlots.Add(EEquipmentSlot::Head, EquipmentSlotHead);
	EquipmentSlots.Add(EEquipmentSlot::Neck, EquipmentSlotNeck);
	EquipmentSlots.Add(EEquipmentSlot::Shoulders, EquipmentSlotShoulders);
	EquipmentSlots.Add(EEquipmentSlot::Chest, EquipmentSlotChest);
	EquipmentSlots.Add(EEquipmentSlot::Hands, EquipmentSlotHands);
	EquipmentSlots.Add(EEquipmentSlot::FingerOne, EquipmentSlotFingerOne);
	EquipmentSlots.Add(EEquipmentSlot::FingerTwo, EquipmentSlotFingerTwo);
	EquipmentSlots.Add(EEquipmentSlot::Waist, EquipmentSlotWaist);
	EquipmentSlots.Add(EEquipmentSlot::Legs, EquipmentSlotLegs);
	EquipmentSlots.Add(EEquipmentSlot::Feet, EquipmentSlotFeet);
	EquipmentSlots.Add(EEquipmentSlot::WeaponMainHand, EquipmentSlotWeaponMainHand);
	EquipmentSlots.Add(EEquipmentSlot::WeaponOffHand, EquipmentSlotWeaponOffHand);

	return Result;
}
