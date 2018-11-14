// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "DungeonCharacter.h"
#include "Equippable.generated.h"

class ADungeonCharacter;

UINTERFACE(MinimalAPI)
class UEquippable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IEquippable
{
	GENERATED_IINTERFACE_BODY()

	virtual void Equip(ADungeonCharacter* Character) = 0;

	virtual void OnEquip(ADungeonCharacter* Character) = 0;
};
