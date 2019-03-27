// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCharacterAnimInstance.h"
#include "DungeonDeathmatch.h"
#include "PlayerCharacter.h"

#include <Kismet/KismetMathLibrary.h>
#include "AnimationProfile.h"

UDungeonCharacterAnimInstance::UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AimYawDeltaLerpStart = 90;
	AimYawDeltaLerpStop = 5;
	AimLerpAlpha = .05f;

	BlendSpaceChangeBlendTime = .4f;

	SlotAnimsBeingPlayed.Add(TTuple<EAnimSlot, int32>(EAnimSlot::Default, 0));
	SlotAnimsBeingPlayed.Add(TTuple<EAnimSlot, int32>(EAnimSlot::UpperBody, 0));
	SlotAnimsBeingPlayed.Add(TTuple<EAnimSlot, int32>(EAnimSlot::MainHand, 0));
	SlotAnimsBeingPlayed.Add(TTuple<EAnimSlot, int32>(EAnimSlot::OffHand, 0));
}

void UDungeonCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningCharacterMovement = Cast<UCharacterMovementComponent>(OwningCharacter->GetMovementComponent());
	}
}

void UDungeonCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningCharacter)
	{
		MovementSpeed = OwningCharacter->GetAnimationComponent()->GetMovementVelocity().Size();
		MovementDirection = OwningCharacter->GetAnimationComponent()->GetMovementDirection();

		if (OwningCharacterMovement)
		{
			bIsFalling = OwningCharacterMovement->IsFalling();
			if (bIsFalling)
			{
				FallTime += DeltaSeconds;
			}
			else
			{
				FallTime = 0;
			}
		}

		bIsReorientingBody = OwningCharacter->GetAnimationComponent()->GetIsReorientingBody();
		bIsCrouched = OwningCharacter->bIsCrouched;
		bIsJumping = OwningCharacter->GetAnimationComponent()->GetIsJumping();
		bIsMontagePlaying = IsAnyMontagePlaying();
		bIsFreeLooking = OwningCharacter->GetAnimationComponent()->GetIsFreeLooking();

		if (MovementSpeed == 0 || bIsFreeLooking)
		{
			bShouldBlendAimOffset = true;
		}

		if (AimYaw < 0)
		{
			bShouldPlayLeftTurnAnimation = true;
		}
		else
		{
			bShouldPlayLeftTurnAnimation = false;
		}

		UpdateAnimationResources();

		// Calculate how much the yaw has changed from the previous frame
		float PreviousAimYawTarget = AimYawTarget;
		AimYawTarget = OwningCharacter->GetAnimationComponent()->GetAimYaw();
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

		AimPitch = OwningCharacter->GetAnimationComponent()->GetAimPitch();
	}
}

void UDungeonCharacterAnimInstance::UpdateAnimationResources()
{
	if (OwningCharacter)
	{
		// Check if any animations or blend spaces have changed since the last frame, and set timers for proper blending
		UBlendSpace* NewStandingMovementBlendSpace = OwningCharacter->GetAnimationComponent()->GetCombatStandingMovementBlendSpace();
		if (StandingMovementBlendSpace == nullptr)
		{
			StandingMovementBlendSpace = NewStandingMovementBlendSpace;
			PreviousStandingMovementBlendSpace = NewStandingMovementBlendSpace;
		}
		else if (NewStandingMovementBlendSpace != StandingMovementBlendSpace)
		{
			PreviousStandingMovementBlendSpace = StandingMovementBlendSpace;
			StandingMovementBlendSpace = NewStandingMovementBlendSpace;
			bDidStandingMovementBlendSpaceChange = true;
			GetWorld()->GetTimerManager().ClearTimer(ResetStandingBlendSpaceChangeHandle);
			GetWorld()->GetTimerManager().SetTimer(ResetStandingBlendSpaceChangeHandle, this, &UDungeonCharacterAnimInstance::ResetStandingBlendSpaceChange, BlendSpaceChangeBlendTime, false);
		}

		UBlendSpace* NewCrouchingMovementBlendSpace = OwningCharacter->GetAnimationComponent()->GetCombatCrouchingMovementBlendSpace();
		if (CrouchingMovementBlendSpace == nullptr)
		{
			CrouchingMovementBlendSpace = NewCrouchingMovementBlendSpace;
			PreviousCrouchingMovementBlendSpace = NewCrouchingMovementBlendSpace;
		}
		else if (NewCrouchingMovementBlendSpace != CrouchingMovementBlendSpace)
		{
			PreviousCrouchingMovementBlendSpace = CrouchingMovementBlendSpace;
			CrouchingMovementBlendSpace = NewCrouchingMovementBlendSpace;
			bDidCrouchingMovementBlendSpaceChange = true;
			GetWorld()->GetTimerManager().ClearTimer(ResetCrouchingBlendSpaceChangeHandle);
			GetWorld()->GetTimerManager().SetTimer(ResetCrouchingBlendSpaceChangeHandle, this, &UDungeonCharacterAnimInstance::ResetCrouchingBlendSpaceChange, BlendSpaceChangeBlendTime, false);
		}

		UAnimSequence* NewJumpingAnimation = OwningCharacter->GetAnimationComponent()->GetCombatJumpingAnimation();
		if (JumpingAnimation == nullptr)
		{
			JumpingAnimation = NewJumpingAnimation;
			PreviousJumpingAnimation = NewJumpingAnimation;
		}
		else if (NewJumpingAnimation != JumpingAnimation)
		{
			PreviousJumpingAnimation = JumpingAnimation;
			JumpingAnimation = NewJumpingAnimation;
			bDidJumpAnimationChange = true;
			GetWorld()->GetTimerManager().ClearTimer(ResetJumpingAnimationChangeHandle);
			GetWorld()->GetTimerManager().SetTimer(ResetJumpingAnimationChangeHandle, this, &UDungeonCharacterAnimInstance::ResetJumpingAnimationChange, BlendSpaceChangeBlendTime, false);
		}

		UBlendSpace1D* NewFallingBlendSpace = OwningCharacter->GetAnimationComponent()->GetCombatFallingBlendSpace();
		if (FallingBlendSpace == nullptr)
		{
			FallingBlendSpace = NewFallingBlendSpace;
			PreviousFallingBlendSpace = NewFallingBlendSpace;
		}
		else if (NewFallingBlendSpace != FallingBlendSpace)
		{
			PreviousFallingBlendSpace = FallingBlendSpace;
			FallingBlendSpace = NewFallingBlendSpace;
			bDidFallingBlendSpaceChange = true;
			GetWorld()->GetTimerManager().ClearTimer(ResetFallingBlendSpaceChangeHandle);
			GetWorld()->GetTimerManager().SetTimer(ResetFallingBlendSpaceChangeHandle, this, &UDungeonCharacterAnimInstance::ResetFallingBlendSpaceChange, BlendSpaceChangeBlendTime, false);
		}

		UBlendSpace* NewLandingBlendSpace = OwningCharacter->GetAnimationComponent()->GetCombatLandingBlendSpace();
		if (LandingBlendSpace == nullptr)
		{
			LandingBlendSpace = NewLandingBlendSpace;
			PreviousLandingBlendSpace = NewLandingBlendSpace;
		}
		else if (NewLandingBlendSpace != LandingBlendSpace)
		{
			PreviousLandingBlendSpace = LandingBlendSpace;
			LandingBlendSpace = NewLandingBlendSpace;
			bDidLandingBlendSpaceChange = true;
			GetWorld()->GetTimerManager().ClearTimer(ResetLandingBlendSpaceChangeHandle);
			GetWorld()->GetTimerManager().SetTimer(ResetLandingBlendSpaceChangeHandle, this, &UDungeonCharacterAnimInstance::ResetLandingBlendSpaceChange, BlendSpaceChangeBlendTime, false);
		}
	}

	for (TTuple<EAnimSlot, int32> Tuple : SlotAnimsBeingPlayed)
	{
		int32* IntPtr = nullptr;
		switch (Tuple.Key)
		{

		case EAnimSlot::Default:
			IntPtr = SlotAnimsBeingPlayed.Find(EAnimSlot::Default);
			if (*(IntPtr) > 0)
			{
				bIsAnimPlayingInDefaultSlot = true;
			}
			else
			{
				bIsAnimPlayingInDefaultSlot = false;
			}
			break;
		case EAnimSlot::UpperBody:
			IntPtr = SlotAnimsBeingPlayed.Find(EAnimSlot::UpperBody);
			if (*(IntPtr) > 0)
			{
				bIsAnimPlayingInUpperBodySlot = true;
			}
			else
			{
				bIsAnimPlayingInUpperBodySlot = false;
			}
			break;
		case EAnimSlot::MainHand:
			IntPtr = SlotAnimsBeingPlayed.Find(EAnimSlot::MainHand);
			if (*(IntPtr) > 0)
			{
				bIsAnimPlayingInMainHandSlot = true;
			}
			else
			{
				bIsAnimPlayingInMainHandSlot = false;
			}
			break;
		case EAnimSlot::OffHand:
			IntPtr = SlotAnimsBeingPlayed.Find(EAnimSlot::OffHand);
			if (*(IntPtr) > 0)
			{
				bIsAnimPlayingInOffHandSlot = true;
			}
			else
			{
				bIsAnimPlayingInOffHandSlot = false;
			}
			break;
		default:
			break;
		}
	}
}

void UDungeonCharacterAnimInstance::SetMontageStartedInSlot(EAnimSlot Slot)
{
	int32* IntPtr = SlotAnimsBeingPlayed.Find(Slot);
	*(IntPtr)++;
}

void UDungeonCharacterAnimInstance::SetMontageStoppedInSlot(EAnimSlot Slot)
{
	int32* IntPtr = SlotAnimsBeingPlayed.Find(Slot);
	*(IntPtr)--;
}

void UDungeonCharacterAnimInstance::ResetStandingBlendSpaceChange()
{
	PreviousStandingMovementBlendSpace = StandingMovementBlendSpace;
	bDidStandingMovementBlendSpaceChange = false;
}

void UDungeonCharacterAnimInstance::ResetCrouchingBlendSpaceChange()
{
	PreviousCrouchingMovementBlendSpace = CrouchingMovementBlendSpace;
	bDidCrouchingMovementBlendSpaceChange = false;
}

void UDungeonCharacterAnimInstance::ResetJumpingAnimationChange()
{
	PreviousJumpingAnimation = JumpingAnimation;
	bDidJumpAnimationChange = false;
}

void UDungeonCharacterAnimInstance::ResetFallingBlendSpaceChange()
{
	PreviousFallingBlendSpace = FallingBlendSpace;
	bDidFallingBlendSpaceChange = false;
}

void UDungeonCharacterAnimInstance::ResetLandingBlendSpaceChange()
{
	PreviousLandingBlendSpace = LandingBlendSpace;
	bDidLandingBlendSpaceChange = false;
}