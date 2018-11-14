// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CanRoll.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UAnimNotify_CanRoll : public UAnimNotify
{
	GENERATED_BODY()
	
	UAnimNotify_CanRoll(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	
};
