// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DungeonCharacter.h"
#include "EquipmentInterface.generated.h"

class ADungeonCharacter;

UINTERFACE(MinimalAPI)
class UEquipmentInterface : public UInterface
{
	friend class UEquipmentComponent;

	GENERATED_UINTERFACE_BODY()
};

class IEquipmentInterface
{
	GENERATED_IINTERFACE_BODY()

protected:

	virtual void NativeOnEquip(ADungeonCharacter* EquippingCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment")
	void OnEquip(ADungeonCharacter* EquippingCharacter);

	virtual void NativeOnUnequip(ADungeonCharacter* UnequippingCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment")
	void OnUnequip(ADungeonCharacter* UnequippingCharacter);
};
