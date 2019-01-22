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

	// Set collision channels for any additional mesh components added in subclasses (for things link boots or gloves where there are separate meshes
	// for each hand or foot)
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetCollisionObjectType(TRACE_INTERACTABLE);
			MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
	}
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
