// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Engine/StreamableManager.h>
#include "Item.h"
#include "Equippable.h"
#include "DungeonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	FStreamableManager AssetLoader;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierColors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EEquipmentSlot, UTexture2D*> EmptyEquipmentSlotIcons;

	/* The initial forward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Globals - Loot")
	float LootEjectionForwardForce;

	/* The initial upward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Globals - Loot")
	float LootEjectionUpForce;

public:
	UDungeonGameInstance(const FObjectInitializer& ObjectInitializer);

	FStreamableManager& GetAssetLoader();

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierColors();

	UFUNCTION(BlueprintPure)
	TMap<EEquipmentSlot, UTexture2D*> GetEmptyEquipmentSlotIcons();

	UFUNCTION(BlueprintPure)
	FVector GetRandomLootEjectionForce();
};
