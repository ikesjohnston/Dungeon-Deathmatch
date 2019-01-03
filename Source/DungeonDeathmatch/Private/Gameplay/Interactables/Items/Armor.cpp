// Fill out your copyright notice in the Description page of Project Settings.

#include "Armor.h"

AArmor::AArmor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMeshComponent"));
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ArmorMeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);
	ArmorMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ArmorMeshComponent->SetupAttachment(GetMeshComponent());
}

AArmor::~AArmor()
{

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
	NewEquippingCharacter->UpdateMeshSegments(ArmorMeshMap);
}
