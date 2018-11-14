// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttachWeapon.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UAnimNotifyState_AttachWeapon : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_AttachWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
