// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MeleeCanDamage.generated.h"

/**
 * This notify state defines a time frame where melee hit events can be thrown
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Check Melee Collisions"))
class DUNGEONDEATHMATCH_API UAnimNotifyState_MeleeCanDamage : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	/**
	 * Is this event for an unarmed attack? These will be processed differently from
	 * normal melee weapons.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	bool bIsUnarmedAttack;

	/**
	 * Is this event for a main handed weapon attack?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	bool bIsMainHandAttack;

public: 
	UAnimNotifyState_MeleeCanDamage(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
