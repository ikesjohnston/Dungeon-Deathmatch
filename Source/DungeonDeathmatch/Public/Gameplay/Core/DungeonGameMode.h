// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Item.h"
#include "Equippable.h"
#include "DungeonGameMode.generated.h"

/*
 * Delegate for raising events when an actor is killed, used for things like
 * adding points to a player's score when killing an enemy.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * Game Mode class defines all specific behavior for a particular game type. Thia code s only run on
 * the server.
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Settings")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierColors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Settings")
	TMap<EEquipmentSlot, UTexture2D*> EmptyEquipmentSlotIcons;

	/* The initial forward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings")
	float LootEjectionForwardForce;

	/* The initial upward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings")
	float LootEjectionUpForce;

public:
	ADungeonGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierColors();

	UFUNCTION(BlueprintPure)
	TMap<EEquipmentSlot, UTexture2D*> GetEmptyEquipmentSlotIcons();

	UFUNCTION(BlueprintPure)
	FVector GetRandomLootEjectionForce();

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:
	void GameOver();
};
