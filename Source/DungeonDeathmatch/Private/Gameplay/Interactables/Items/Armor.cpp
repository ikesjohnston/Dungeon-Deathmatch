// Fill out your copyright notice in the Description page of Project Settings.

#include "Armor.h"

#include <Components/SkeletalMeshComponent.h>
#include <WidgetComponent.h>
#include "ModularHumanoidMeshComponent.h"

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

TMap<EHumanoidMeshSegment, USkeletalMesh*> AArmor::GetArmorMeshMap()
{
	return ArmorMeshMap;
}

void AArmor::ServerOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	if (HasAuthority())
	{
		Super::ServerOnEquip_Implementation(InEquippingActor, EquipmentSlot);
		UModularHumanoidMeshComponent* HumanoidMeshComponent = Cast<UModularHumanoidMeshComponent>(InEquippingActor->GetComponentByClass(UModularHumanoidMeshComponent::StaticClass()));
		if (HumanoidMeshComponent)
		{
			for (TTuple<EHumanoidMeshSegment, USkeletalMesh*> Tuple : ArmorMeshMap)
			{
				HumanoidMeshComponent->ServerUpdateMeshSegment(Tuple.Key, Tuple.Value);
			}
		}
	}
}

void AArmor::MulticastOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(InEquippingActor, EquipmentSlot);
}

void AArmor::ServerOnUnequip_Implementation()
{
	if (HasAuthority())
	{
		UModularHumanoidMeshComponent* HumanoidMeshComponent = Cast<UModularHumanoidMeshComponent>(EquippingActor->GetComponentByClass(UModularHumanoidMeshComponent::StaticClass()));
		if (HumanoidMeshComponent)
		{
			for (TTuple<EHumanoidMeshSegment, USkeletalMesh*> Tuple : ArmorMeshMap)
			{
				HumanoidMeshComponent->ServerUpdateMeshSegment(Tuple.Key, nullptr);
			}
		}
		Super::ServerOnUnequip_Implementation();
	}
}

void AArmor::MulticastOnUnequip_Implementation()
{
	Super::MulticastOnUnequip_Implementation();
}