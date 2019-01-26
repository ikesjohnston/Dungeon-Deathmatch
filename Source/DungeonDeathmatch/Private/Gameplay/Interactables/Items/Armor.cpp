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

void AArmor::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	NewEquippingCharacter->Server_UpdateMeshSegments(this);
}
