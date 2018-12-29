// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacterAnimInstance.h"
#include "DungeonDeathmatch.h"
#include <Kismet/KismetMathLibrary.h>
#include "AnimationProfile.h"

UDungeonCharacterAnimInstance::UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
		AimYaw = OwningCharacter->GetAimYaw();
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

