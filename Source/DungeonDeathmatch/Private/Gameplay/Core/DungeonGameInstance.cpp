// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierColors()
{
	return ItemQualityTierColors;
}

TMap<EEquipmentSlot, UTexture2D*> UDungeonGameInstance::GetEmptyEquipmentSlotIcons()
{
	return EmptyEquipmentSlotIcons;
}

FVector UDungeonGameInstance::GetRandomLootEjectionForce()
{
	// Get a random forward ejection vector
	FVector EjectionVector = FVector(LootEjectionForwardForce, 0, LootEjectionUpForce);
	float RandomForwardAngle = FMath::FRand() * 360.0f;
	EjectionVector.RotateAngleAxis(RandomForwardAngle, FVector::UpVector);

	return EjectionVector;
}
