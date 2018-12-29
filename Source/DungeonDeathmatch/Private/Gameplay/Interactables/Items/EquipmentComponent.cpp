// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, Equipment);
}

TArray<AEquippable*> UEquipmentComponent::GetEquipment() const
{
	return Equipment;
}

AEquippable* UEquipmentComponent::GetEquipmentInSlot(EEquipmentSlot Slot) const
{
	return Equipment[(int32) Slot];
}

void UEquipmentComponent::Multicast_OnEquipItem_Implementation(AEquippable* Equippable)
{
	Equippable->GetMeshComponent()->SetVisibility(true);

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	AttachRules.bWeldSimulatedBodies = false;

	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (OwningCharacter)
	{
		Equippable->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, "HipSheatheSocket");
	}
}

void UEquipmentComponent::Server_EquipItem_Implementation(AEquippable* Equippable)
{
	if (!HasItemEquipped(Equippable))
	{
		Equipment.Insert(Equippable, (int32) EEquipmentSlot::MainHand);
		Multicast_OnEquipItem(Equippable);
		// test code, to remove 
		ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
		if (Character)
		{
//				Item->Server_OnEquip(Character);
		}
	}
}

bool UEquipmentComponent::Server_EquipItem_Validate(AEquippable* Equippable)
{
	return true;
}

void UEquipmentComponent::Server_UnequipItem_Implementation(AEquippable* Equippable)
{
	if (!HasItemEquipped(Equippable))
	{
		Equipment.RemoveSwap(Equippable);
		Multicast_OnUnequipItem(Equippable);
		OnItemUnequipped.ExecuteIfBound(Equippable);
	}
}

bool UEquipmentComponent::Server_UnequipItem_Validate(AEquippable* Equippable)
{
	return true;
}

void UEquipmentComponent::Multicast_OnUnequipItem_Implementation(AEquippable* Equippable)
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	Equippable->DetachFromActor(DetachRules);
}

void UEquipmentComponent::Server_UnequipAll_Implementation()
{
	while (Equipment.Num() > 0)
	{
		Server_UnequipItem(Equipment[0]);
	}
}

bool UEquipmentComponent::Server_UnequipAll_Validate()
{
	return true;
}

bool UEquipmentComponent::HasItemEquipped(AEquippable* Equippable) const
{
	for (AEquippable* EquippedItem : Equipment)
	{
		if (EquippedItem == Equippable)
		{
			return true;
		}
	}

	return false;
}

bool UEquipmentComponent::TryEquipItem(AEquippable* Equippable)
{
	return true;
}

bool UEquipmentComponent::TryUnequipItem(AEquippable* Equippable)
{
	return true;
}

