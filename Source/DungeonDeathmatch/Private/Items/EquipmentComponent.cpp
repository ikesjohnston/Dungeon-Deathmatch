// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "EquipmentInterface.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	bReplicates = true;

	Equipment.SetNum((int32) EEquipmentSlot::NUM_EQUIPMENT_SLOTS);
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

TArray<AItem*> UEquipmentComponent::GetEquipment() const
{
	return Equipment;
}

AItem* UEquipmentComponent::GetEquipmentInSlot(EEquipmentSlot Slot) const
{
	return Equipment[(int32) Slot];
}

void UEquipmentComponent::Multicast_OnEquipItem_Implementation(AItem* Item)
{
	Item->GetMeshComponent()->SetVisibility(true);

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	AttachRules.bWeldSimulatedBodies = false;

	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (OwningCharacter)
	{
		Item->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, "HipSheatheSocket");
	}
}

void UEquipmentComponent::Server_EquipItem_Implementation(AItem* Item)
{
	if (Cast<IEquipmentInterface>(Item))
	{
		if (!HasItemEquipped(Item))
		{
			Equipment.Insert(Item, (int32) EEquipmentSlot::MainHand);
			Multicast_OnEquipItem(Item);
			// test code, to remove 
			ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
			if (Character)
			{
//				Item->Server_OnEquip(Character);
			}
		}
	}
}

bool UEquipmentComponent::Server_EquipItem_Validate(AItem* Item)
{
	return true;
}

void UEquipmentComponent::Server_UnequipItem_Implementation(AItem* Item)
{
	if (Cast<IEquipmentInterface>(Item))
	{
		if (!HasItemEquipped(Item))
		{
			Equipment.RemoveSwap(Item);
			Multicast_OnUnequipItem(Item);
			OnItemUnequipped.ExecuteIfBound(Item);
		}
	}
}

bool UEquipmentComponent::Server_UnequipItem_Validate(AItem* Item)
{
	return true;
}

void UEquipmentComponent::Multicast_OnUnequipItem_Implementation(AItem* Item)
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	Item->DetachFromActor(DetachRules);
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

bool UEquipmentComponent::HasItemEquipped(AItem* Item) const
{
	for (AItem* EquippedItem : Equipment)
	{
		if (EquippedItem == Item)
		{
			return true;
		}
	}

	return false;
}

