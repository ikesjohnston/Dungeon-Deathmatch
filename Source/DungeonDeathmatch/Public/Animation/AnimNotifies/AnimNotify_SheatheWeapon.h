// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SheatheWeapon.generated.h"

/**
 * AnimNotify that attaches a character's weapon to a specific weapon sheathe
 */
UCLASS(meta = (DisplayName = "Sheathe Weapon"))
class DUNGEONDEATHMATCH_API UAnimNotify_SheatheWeapon : public UAnimNotify
{
	GENERATED_BODY()

protected:
	/** Should the off hand weapon be sheathed instead of the main hand? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify")
	bool bSheatheOffHand;

public:
	UAnimNotify_SheatheWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
};
