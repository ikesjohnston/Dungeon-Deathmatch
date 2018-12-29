// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "Interactable.h"
#include "InteractionComponent.h"
#include "Chest.generated.h"

class USkeletalMeshComponent;
class UWidgetComponent;
class UInteractionComponent;
class ULootComponent;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API AChest : public AActor, public IInteractable
{
	GENERATED_BODY()

	// Only the anim notify should be able to call the loot function, we don't want to call it directly 
	// in case animations haven't finished playing
	friend class UAnimNotify_ChestLoot;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UWidgetComponent* WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot")
	ULootComponent* LootComponent;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsOpened)
	bool bIsOpened;

public:
	AChest();

	bool GetIsOpened();

	virtual void OnInteract_Implementation(ADungeonCharacter* InteractingCharacter) override;

	virtual void OnFocused() override;

	virtual void OnUnfocused() override;

	virtual UWidgetComponent* GetWidgetComponent() override;

	virtual bool GetCanInteract() override;

	virtual void SetCanInteract(bool CanInteract) override;

	virtual FText GetInteractionPromptText() override;

	virtual FText GetInteractableName() override;

protected:
	UFUNCTION()
	void OnRep_IsOpened();

	void Loot();
};
