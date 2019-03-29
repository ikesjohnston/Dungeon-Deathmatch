// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SwingWeapon.generated.h"

class AWeapon;

/**
 * 
 */
UCLASS(meta = (DisplayName = "Swing Weapon"))
class DUNGEONDEATHMATCH_API UAnimNotifyState_SwingWeapon : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	/** Should the off hand weapon be swung instead of the main hand? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	bool bSwingOffHand;

private:
	AWeapon* SwingingWeapon;

public:
	UAnimNotifyState_SwingWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
