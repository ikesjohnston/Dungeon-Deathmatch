// Fill out your copyright notice in the Description page of Project Settings.

#include "Armor.h"
#include <Components/SkeletalMeshComponent.h>
#include <WidgetComponent.h>

AArmor::AArmor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

AArmor::~AArmor()
{

}

void AArmor::BeginPlay()
{
	Super::BeginPlay();
}

EArmorSlot AArmor::GetArmorSlot()
{
	return ArmorSlot;
}

TMap<EMeshSegment, USkeletalMesh*> AArmor::GetArmorMeshMap()
{
	return ArmorMeshMap;
}

void AArmor::ServerOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	if (Role == ROLE_Authority)
	{
		Super::ServerOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);
		for (TTuple<EMeshSegment, USkeletalMesh*> Tuple : ArmorMeshMap)
		{
			EquippingCharacter->GetModularCharacterMeshComponent()->ServerUpdateMeshSegment(Tuple.Key, Tuple.Value);
		}
	}
}

void AArmor::MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);
}

void AArmor::ServerOnUnequip_Implementation()
{
	if (Role == ROLE_Authority)
	{
		for (TTuple<EMeshSegment, USkeletalMesh*> Tuple : ArmorMeshMap)
		{
			EquippingCharacter->GetModularCharacterMeshComponent()->ServerUpdateMeshSegment(Tuple.Key, nullptr);
		}
		Super::ServerOnUnequip_Implementation();
	}
}

void AArmor::MulticastOnUnequip_Implementation()
{
	Super::MulticastOnUnequip_Implementation();
}