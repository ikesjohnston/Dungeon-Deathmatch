// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacterAnimInstance.h"
#include "DungeonDeathmatch.h"
#include <Kismet/KismetMathLibrary.h>
#include "AnimationProfile.h"

UDungeonCharacterAnimInstance::UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AimYawDeltaLerpStart = 90;
	AimYawDeltaLerpStop = 5;
	AimLerpAlpha = .05f;
}

void UDungeonCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<ADungeonCharacter>(TryGetPawnOwner());
}

void UDungeonCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningCharacter)
	{
		MovementSpeed = OwningCharacter->GetMovementVelocity().Size();
		MovementDirection = OwningCharacter->GetMovementDirection();

		bIsReorientingBody = OwningCharacter->GetIsReorientingBody();

		// Calculate how much the yaw has changed from the previous frame
		float PreviousAimYawTarget = AimYawTarget;
		AimYawTarget = OwningCharacter->GetAimYaw();
		// If it has exceeded the defined threshold, we will lerp to the aim offset to prevent jerky aiming animations
		if (FMath::Abs(PreviousAimYawTarget - AimYawTarget) > AimYawDeltaLerpStart)
		{
			bIsLerpingAim = true;
		}
		// Stop lerping once we get within a defined range, so future yaw aim animations aren't slowed from the lerping
		else if (bIsLerpingAim && FMath::Abs(PreviousAimYawTarget - AimYawTarget) > 0 && FMath::Abs(PreviousAimYawTarget - AimYawTarget) <= AimYawDeltaLerpStop)
		{
			bIsLerpingAim = false;
		}

		if (bIsLerpingAim)
		{
			AimYaw = FMath::Lerp(AimYaw, AimYawTarget, AimLerpAlpha);
		}
		else
		{
			// Just use the calculated aim offset from the character if it is within the threshold
			AimYaw = AimYawTarget;
		}

		AimPitch = OwningCharacter->GetAimPitch();
	}
}

UBlendSpace* UDungeonCharacterAnimInstance::GetMovementBlendSpace()
{
	UBlendSpace* BlendSpace = nullptr;

	if (OwningCharacter)
	{
		UAnimationProfile* AnimProfile = OwningCharacter->GetAnimationProfile();
		if (AnimProfile)
		{
			BlendSpace = AnimProfile->GetMovementBlendSpace();
		}
	}

	return BlendSpace;
}

