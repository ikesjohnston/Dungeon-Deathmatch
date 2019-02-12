// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimGlobals.h"
#include "AnimNotifyState_SlotAnimPlaying.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UAnimNotifyState_SlotAnimPlaying : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	/** The slot that the animation occupies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	EAnimSlot AnimSlot;

public:
	UAnimNotifyState_SlotAnimPlaying(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
