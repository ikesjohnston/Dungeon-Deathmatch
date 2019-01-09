// Fill out your copyright notice in the Description page of Project Settings.

#include "Equippable.h"

AEquippable::AEquippable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

AEquippable::~AEquippable()
{

}

FText AEquippable::GetInventoryUseTooltipText()
{
	if (EquippingCharacter)
	{
		return FText::FromString("Unequip");
	}

	return FText::FromString("Equip");
}

void AEquippable::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	if (Role == ROLE_Authority)
	{
		InteractingCharacter->Server_TryEquipItem(this);
	}
}

void AEquippable::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	EquippingCharacter = NewEquippingCharacter;
}

void AEquippable::OnUnequip_Implementation()
{
	EquippingCharacter = nullptr;
}
