// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentInterface.h"

UEquipmentInterface::UEquipmentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IEquipmentInterface::NativeOnEquip(ADungeonCharacter* EquippingCharacter)
{
	// Call blueprint scriptable event
	Execute_OnEquip(_getUObject(), EquippingCharacter);
}

void IEquipmentInterface::NativeOnUnequip(ADungeonCharacter* UnequippingCharacter)
{
	// Call blueprint scriptable event
	Execute_OnUnequip(_getUObject(), UnequippingCharacter);
}