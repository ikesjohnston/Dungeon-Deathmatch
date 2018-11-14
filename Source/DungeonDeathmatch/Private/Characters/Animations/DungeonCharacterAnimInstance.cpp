// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacterAnimInstance.h"
#include "DungeonDeathmatch.h"
#include <Kismet/KismetMathLibrary.h>

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
		UCharacterMovementComponent* MovementComp = OwningCharacter->GetCharacterMovement();

		FVector Velocity = MovementComp->Velocity;
		MovementSpeed = Velocity.Size();

		FRotator Rotation = OwningCharacter->GetActorRotation();
		FVector LocalVelocity = Rotation.UnrotateVector(Velocity);
		MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
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

UBlendSpace* UDungeonCharacterAnimInstance::GetJumpBlendSpace()
{
	UBlendSpace* BlendSpace = nullptr;

	if (OwningCharacter)
	{
		UAnimationProfile* AnimProfile = OwningCharacter->GetAnimationProfile();
		if (AnimProfile)
		{
			BlendSpace = AnimProfile->GetJumpBlendSpace();
		}
	}

	return BlendSpace;
}

UBlendSpace* UDungeonCharacterAnimInstance::GetLandBlendSpace()
{
	UBlendSpace* BlendSpace = nullptr;

	if (OwningCharacter)
	{
		UAnimationProfile* AnimProfile = OwningCharacter->GetAnimationProfile();
		if (AnimProfile)
		{
			BlendSpace = AnimProfile->GetLandBlendSpace();
		}
	}

	return BlendSpace;
}

