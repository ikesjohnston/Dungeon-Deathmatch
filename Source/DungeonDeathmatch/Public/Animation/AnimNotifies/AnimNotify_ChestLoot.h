// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Chest.h"
#include "AnimNotify_ChestLoot.generated.h"

/**
 * This notify tells a chest that it can start ejecting any generated loot
 */
UCLASS()
class DUNGEONDEATHMATCH_API UAnimNotify_ChestLoot : public UAnimNotify
{
	GENERATED_BODY()

public:

	UAnimNotify_ChestLoot(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
};
