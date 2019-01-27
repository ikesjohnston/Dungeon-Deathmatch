// Fill out your copyright notice in the Description page of Project Settings.

#include "Equippable.h"

AEquippable::AEquippable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

AEquippable::~AEquippable()
{

}

void AEquippable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquippable, EquippingCharacter);
}

FText AEquippable::GetInventoryUseTooltipText()
{
	if (EquippingCharacter)
	{
		return FText::FromString("Unequip");
	}

	return FText::FromString("Equip");
}

void AEquippable::Server_OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	ServerOnEquip(NewEquippingCharacter);
	Multicast_OnEquip(NewEquippingCharacter);
}

bool AEquippable::Server_OnEquip_Validate(ADungeonCharacter* NewEquippingCharacter)
{
	return true;
}

void AEquippable::ServerOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	EquippingCharacter = NewEquippingCharacter;
}

void AEquippable::Multicast_OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{

}

void AEquippable::MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	
}

void AEquippable::Server_OnUnequip_Implementation()
{
	ServerOnUnequip();
	Multicast_OnUnequip();
}

bool AEquippable::Server_OnUnequip_Validate()
{
	return true;
}

void AEquippable::ServerOnUnequip_Implementation()
{
	EquippingCharacter = nullptr;
}

void AEquippable::Multicast_OnUnequip_Implementation()
{
	MulticastOnUnequip();
}

void AEquippable::MulticastOnUnequip_Implementation()
{

}
