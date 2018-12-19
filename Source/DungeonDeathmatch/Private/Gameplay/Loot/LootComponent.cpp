// Fill out your copyright notice in the Description page of Project Settings.

#include "LootComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Engine/Engine.h>

ULootComponent::ULootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bWasLootGenerated = false;
}

void ULootComponent::GenerateLoot()
{
	// Only generate loot on the server
	if (GetOwner()->Role == ROLE_Authority)
	{
		if (!bWasLootGenerated)
		{
			static const FString ContextString(TEXT("GENERAL"));
			TArray<FLootTableRow*> TableRows;
			LootTable->GetAllRows(ContextString, TableRows);

			float RandomWeightTotal = 0.0f;
			for (FLootTableRow* Loot : TableRows)
			{
				RandomWeightTotal += Loot->DropChanceWeight;
			}

			int32 DropQuantity = UKismetMathLibrary::RandomIntegerInRange(DropQuantityMin, DropQuantityMax);
			UE_LOG(LogTemp, Log, TEXT("ULootComponent::GenerateLoot - Generating %d loot item(s)"), DropQuantity);
			for (int DropIndex = 0; DropIndex < DropQuantity; DropIndex++)
			{
				// Get random item based on an item's drop weight relative to the combined total drop weight of all items.
				float TotalWeight = 0.0f;
				float WeightValue = FMath::FRandRange(0.0f, RandomWeightTotal);
				for (FLootTableRow* Loot : TableRows)
				{
					if (WeightValue > TotalWeight && WeightValue <= TotalWeight + Loot->DropChanceWeight)
					{
						UE_LOG(LogTemp, Log, TEXT("ULootComponent::GenerateLoot - Dropped %s"), *Loot->ItemName);
						// Don't add the "Nothing" item entry to the array of items to spawn
						if (Loot->ItemID >= 0)
						{
							GeneratedLootClasses.Add(Loot->ItemClass);
						}
						break;
					}
					TotalWeight += Loot->DropChanceWeight;
				}
			}
		}

		EjectLoot();
		bWasLootGenerated = true;
	}
}

void ULootComponent::EjectLoot()
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (GeneratedLootClasses.Num() > 0)
		{
			TSubclassOf<AItem> ItemClass = GeneratedLootClasses.Pop();

			FActorSpawnParameters ItemSpawnParams;
			ItemSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			float ItemSpawnPitch = FMath::FRand() * 360.0;
			float ItemSpawnYaw = FMath::FRand() * 360.0;
			float ItemSpawnRoll = FMath::FRand() * 360.0;
			FRotator ItemSpawnRotation = FRotator(ItemSpawnPitch, ItemSpawnYaw, ItemSpawnRoll);

			AItem* SpawnedItem = Owner->GetWorld()->SpawnActor<AItem>(ItemClass, Owner->GetActorLocation() + FVector(0, 0, 100), ItemSpawnRotation);
			UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(Owner->GetGameInstance());
			if (GameInstance)
			{
				// Just apply a global force vector for ejection, no need to use temporary actor components
				SpawnedItem->GetMeshComponent()->AddForce(GameInstance->GetRandomLootEjectionForce());
			}

			// Call eject loot again until we run out of items
			Owner->GetWorldTimerManager().SetTimer(LootEjectionHandle, this, &ULootComponent::EjectLoot, LootEjectionDelay, false);
		}
	}
}

void ULootComponent::SimulateLootDrops(int32 NumberOfDrops)
{
	// Only generate loot on the server
	if (GetOwner()->Role == ROLE_Authority)
	{
		static const FString ContextString(TEXT("GENERAL"));
		TArray<FLootTableRow*> TableRows;
		LootTable->GetAllRows(ContextString, TableRows);

		TMap<FLootTableRow*, int32> Drops;
		float RandomWeightTotal = 0.0f;
		for (FLootTableRow* Loot : TableRows)
		{
			RandomWeightTotal += Loot->DropChanceWeight;
			Drops.Add(Loot, 0);
		}

		UE_LOG(LogTemp, Log, TEXT("ULootComponent::SimulateLootDrops - Simulating %d loot drop(s)"), NumberOfDrops);
		for (int DropIndex = 0; DropIndex < NumberOfDrops; DropIndex++)
		{
			// Get random item based on an item's drop weight relative to the combined total drop weight of all items.
			float TotalWeight = 0.0f;
			float WeightValue = FMath::FRandRange(0.0f, RandomWeightTotal);
			for (FLootTableRow* Loot : TableRows)
			{
				if (WeightValue > TotalWeight && WeightValue <= TotalWeight + Loot->DropChanceWeight)
				{
					int32* DropQuantity = Drops.Find(Loot);
					*(DropQuantity) += 1;

					break;
				}
				TotalWeight += Loot->DropChanceWeight;
			}
		}

		for (FLootTableRow* Loot : TableRows)
		{
			UE_LOG(LogTemp, Log, TEXT("ULootComponent::SimulateLootDrops - %s was dropped %d / %d time(s) (%.3f percent)"), *(Loot->ItemName), *(Drops.Find(Loot)), NumberOfDrops, (*(Drops.Find(Loot)) / (float)NumberOfDrops) * 100.0f);
		}
	}
}

