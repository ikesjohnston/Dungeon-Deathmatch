// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Engine/StreamableManager.h>
#include "Item.h"
#include "Equippable.h"
#include "DungeonGameInstance.generated.h"

class UDraggableItemWidget;

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
	/** Mapping of colors used in inventory and equipment menu widgets for different quality items */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierColors;

	/** Mapping of colors used in text for different quality items */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierTextColors;

	/** The size of inventory gird slots, which also effects the size of equipment slots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	float InventoryGridSlotSize;

	/* The blueprint UDragAndDropItemWidget subclass to use for inventory and equipment menu interactions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TSubclassOf<UDraggableItemWidget> DragAndDropItemWidgetClass;

	/* The initial forward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Globals - Loot")
	float LootEjectionForwardForce;

	/* The initial upward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Globals - Loot")
	float LootEjectionUpForce;

	/** Mapping of animation montages to play when sheathing different weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Globals")
	TMap<EWeaponSocketType, UAnimMontage*> SheatheAnimationMontages;

	/** Mapping of animation montages to play when unsheathing different weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Globals")
	TMap<EWeaponSocketType, UAnimMontage*> UnsheatheAnimationMontages;

public:
	UDungeonGameInstance(const FObjectInitializer& ObjectInitializer);

	FStreamableManager& GetAssetLoader();

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierColors();

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierTextColors();

	UFUNCTION(BlueprintPure)
	TSubclassOf<UDraggableItemWidget> GetDragAndDropItemWidgetClass();

	UFUNCTION(BlueprintPure)
	TMap<EWeaponSocketType, UAnimMontage*> GetSheatheAnimationMontages();

	UFUNCTION(BlueprintPure)
	TMap<EWeaponSocketType, UAnimMontage*> GetUnsheatheAnimationMontages();

	UFUNCTION(BlueprintPure)
	float GetInventoryGridSlotSize();

	UFUNCTION(BlueprintPure)
	FVector GetRandomLootEjectionForce();
};
