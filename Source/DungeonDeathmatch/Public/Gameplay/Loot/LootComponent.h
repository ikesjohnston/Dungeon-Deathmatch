// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine/DataTable.h>
#include "LootComponent.generated.h"

class AItem;
class UDataTable;

/**
 * Structure to store possible lootable objects in a UDataTable.
 */
USTRUCT(Blueprintable)
struct FLootTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/* Item ID to be used for lookups, safer than name lookups. An ID of -1 corresponds to no item dropping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = -1))
	int32 ItemID;

	/* The name of this item, used to aid designers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	FString ItemName;

	/* A description of this item, used to aid designers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	FString ItemDescription;

	/* Whether this loot option is currently enabled. Disabled items will be ignored on loot generation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	bool bIsEnabled;

	/* 
	 * Whether this loot option is guaranteed to drop. If guaranteed, this item will not count against
	 * the count of loot items generated. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	bool bIsGuaranteed;

	/* Whether this loot option should only be dropped a maximum of one time during loot generation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	bool bIsUnique;

	/* The drop weight value for this item when compared to all other items in the loot table. A higher value will increase the chances of dropping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = 0.0f))
	float DropChanceWeight;

	/* The class of this item to instantiate on drop. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	TSubclassOf<AItem> ItemClass;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API ULootComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/* Minimum quantity of loot to be generated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	uint8 DropQuantityMin;

	/* Maximum quantity of loot to be generated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	uint8 DropQuantityMax;

	/* The data table contatining all possible loot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot")
	UDataTable* LootTable;

	/* The time in seconds between each piece of generated loot being ejected from the loot source */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	float LootEjectionDelay;

private:
	bool bWasLootGenerated;

	/* Array of the item classes to instantiate at the end of loot generation */
	TArray<TSubclassOf<AItem>> GeneratedLootClasses;

	/* Timer handle for loot ejection delay loops */
	FTimerHandle LootEjectionHandle;

public:	
	ULootComponent();

	/**
	 * Generates a random amount of loot based on the component's loot table
	 */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void GenerateLoot();

	/**
	 * Debug function used for testing drop rates of items
	 * @param NumberOfDrops The number of drops to simulate
	 */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void SimulateLootDrops(int32 NumberOfDrops);

private:
	/**
	 * Recursively ejects all generated loot items from the loot source
	 */
	void EjectLoot();
};
