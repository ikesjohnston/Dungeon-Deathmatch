// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimationProfile.h"


UAnimationProfile::UAnimationProfile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAnimMontage* UAnimationProfile::GetDrawWeaponMontage() const
{
	return DrawWeaponMontage;
}

UAnimMontage* UAnimationProfile::GetSheatheWeaponMontage() const
{
	return SheatheWeaponMontage;
}

UAnimMontage* UAnimationProfile::GetAttackOneMontage() const
{
	return AttackOneMontage;
}

UAnimMontage* UAnimationProfile::GetAttackTwoMontage() const
{
	return AttackTwoMontage;
}

UAnimMontage* UAnimationProfile::GetAttackThreeMontage() const
{
	return AttackThreeMontage;
}

UBlendSpace* UAnimationProfile::GetMovementBlendSpace() const
{
	return MovementBlendSpace;
}

UBlendSpace* UAnimationProfile::GetJumpBlendSpace() const
{
	return JumpBlendSpace;
}

UAnimMontage* UAnimationProfile::GetFallingMontage() const
{
	return FallingMontage;
}

UBlendSpace* UAnimationProfile::GetLandBlendSpace() const
{
	return LandBlendSpace;
}