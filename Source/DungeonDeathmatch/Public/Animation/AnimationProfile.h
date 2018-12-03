// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Animation/AnimMontage.h>
#include <Animation/BlendSpace.h>
#include "AnimationProfile.generated.h"

/**
 * An Animation Profile defines the core animation montages for a character.
 * A character should have a default animation profile, which can then have 
 * individual animations overridden by equipped weapons.
 */
UCLASS()
class UAnimationProfile : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Combat")
	UAnimMontage* DrawWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Combat")
	UAnimMontage* SheatheWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Combat")
	UAnimMontage* AttackOneMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Combat")
	UAnimMontage* AttackTwoMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Combat")
	UAnimMontage* AttackThreeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Locomotion")
	UBlendSpace* MovementBlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Locomotion")
	UBlendSpace* JumpBlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Locomotion")
	UAnimMontage* FallingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation - Locomotion")
	UBlendSpace* LandBlendSpace;

public:
	UAnimationProfile(const FObjectInitializer& ObjectInitializer);

	UAnimMontage* GetDrawWeaponMontage() const;

	UAnimMontage* GetSheatheWeaponMontage() const;

	UAnimMontage* GetAttackOneMontage() const;

	UAnimMontage* GetAttackTwoMontage() const;

	UAnimMontage* GetAttackThreeMontage() const;

	UBlendSpace* GetMovementBlendSpace() const;

	UBlendSpace* GetJumpBlendSpace() const;

	UAnimMontage* GetFallingMontage() const;

	UBlendSpace* GetLandBlendSpace() const;
};
