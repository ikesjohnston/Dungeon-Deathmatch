// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CanChainMeleeCombo.generated.h"

/**
 * This notify starts a timer that allows a character's melee attack ability to be chained to the next in a combo
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Can Chain Melee Combo"))
class DUNGEONDEATHMATCH_API UAnimNotify_CanChainMeleeCombo : public UAnimNotify
{
	GENERATED_BODY()

public:
	/**
	 * How long should the character have to perform the next melee attack in the combo?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (ClampMin = 0.0f))
	float meleeComboOpeningTime;

	UAnimNotify_CanChainMeleeCombo(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
};
