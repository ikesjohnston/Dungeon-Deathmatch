// Fill out your copyright notice in the Description page of Project Settings.

#include "Equippable.h"

AEquippable::AEquippable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

AEquippable::~AEquippable()
{

}

void AEquippable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquippable, EquippingActor);
}

FText AEquippable::GetInventoryUseTooltipText()
{
	if (EquippingActor)
	{
		return FText::FromString("Unequip");
	}

	return FText::FromString("Equip");
}

void AEquippable::ServerOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	ServerOnEquipEvent(InEquippingActor, EquipmentSlot);
	MulticastOnEquip(InEquippingActor, EquipmentSlot);
}

bool AEquippable::ServerOnEquip_Validate(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	return true;
}

void AEquippable::ServerOnEquipEvent_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	EquippingActor = InEquippingActor;
	Execute_SetCanInteract(this, false);
}

void AEquippable::MulticastOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	MulticastOnEquipEvent(InEquippingActor, EquipmentSlot);
}

void AEquippable::MulticastOnEquipEvent_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	
}

void AEquippable::ServerOnUnequip_Implementation()
{
	ServerOnUnequipEvent();
	MulticastOnUnequip();
}

bool AEquippable::ServerOnUnequip_Validate()
{
	return true;
}

void AEquippable::ServerOnUnequipEvent_Implementation()
{
	EquippingActor = nullptr;
	Execute_SetCanInteract(this, true);
}

void AEquippable::MulticastOnUnequip_Implementation()
{
	MulticastOnUnequipEvent();
}

void AEquippable::MulticastOnUnequipEvent_Implementation()
{

}
