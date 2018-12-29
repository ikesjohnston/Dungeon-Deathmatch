// Fill out your copyright notice in the Description page of Project Settings.

#include "Equippable.h"

AEquippable::AEquippable()
{

}

AEquippable::~AEquippable()
{

}

TArray<EEquipmentSlot> AEquippable::GetValidEquipmentSlots()
{
	return TArray<EEquipmentSlot>();
}

TArray<EEquipmentSlot> AEquippable::GetLockedEquipmentSlots()
{
	return TArray<EEquipmentSlot>();
}

FText AEquippable::GetInventoryUseTooltipText()
{
	if (EquippingCharacter)
	{
		return FText::FromString("Unequip");
	}

	return FText::FromString("Equip");
}

void AEquippable::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	EquippingCharacter = NewEquippingCharacter;
}

void AEquippable::OnUnequip_Implementation()
{
	EquippingCharacter = nullptr;
}
