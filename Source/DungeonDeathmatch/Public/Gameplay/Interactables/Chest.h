// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "Gameplay/Interactables/Interactable.h"
#include "Chest.generated.h"

class USkeletalMeshComponent;
class ULootComponent;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API AChest : public AInteractable
{
	GENERATED_BODY()

	// Only the anim notify should be able to call the loot function, we don't want to call it directly 
	// in case animations haven't finished playing
	friend class UAnimNotify_ChestLoot;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* ChestMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	ULootComponent* LootComponent;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsOpened)
	bool bIsOpened;

public:
	AChest();

	bool GetIsOpened();

protected:
	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter) override;

	UFUNCTION()
	void OnRep_IsOpened();

	void Loot();
};
