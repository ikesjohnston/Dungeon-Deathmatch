// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridSlotSize = 40.0f;
}

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierColors()
{
	return ItemQualityTierColors;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierTextColors()
{
	return ItemQualityTierTextColors;
}

TSubclassOf<UDraggableItemWidget> UDungeonGameInstance::GetDragAndDropItemWidgetClass()
{
	return DragAndDropItemWidgetClass;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetSheatheAnimationMontages()
{
	return SheatheAnimationMontages;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetUnsheatheAnimationMontages()
{
	return UnsheatheAnimationMontages;
}

float UDungeonGameInstance::GetInventoryGridSlotSize()
{
	return InventoryGridSlotSize;
}

FVector UDungeonGameInstance::GetRandomLootEjectionForce()
{
	// Get a random forward ejection vector
	FVector EjectionVector = FVector(LootEjectionForwardForce, 0, LootEjectionUpForce);
	float RandomForwardAngle = FMath::FRand() * 360.0f;
	EjectionVector.RotateAngleAxis(RandomForwardAngle, FVector::UpVector);

	return EjectionVector;
}
