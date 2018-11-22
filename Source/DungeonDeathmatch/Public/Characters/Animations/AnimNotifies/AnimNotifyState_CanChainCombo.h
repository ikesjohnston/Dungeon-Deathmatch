// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CanChainCombo.generated.h"

/**
 * This notify state defines a time frame when an ability can be chained to the next in a combo
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Can Chain Combo"))
class DUNGEONDEATHMATCH_API UAnimNotifyState_CanChainCombo : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_CanChainCombo(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
