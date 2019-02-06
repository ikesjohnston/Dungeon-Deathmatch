// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_UnsheatheWeapon.generated.h"

/**
 * AnimNotify that attaches a character's weapon to a specific hand
 */
UCLASS()
class DUNGEONDEATHMATCH_API UAnimNotify_UnsheatheWeapon : public UAnimNotify
{
	GENERATED_BODY()

protected:
	/** Should the off hand weapon be unsheathed instead of the main hand? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	bool bUnsheatheOffHand;

public:
	UAnimNotify_UnsheatheWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
};
