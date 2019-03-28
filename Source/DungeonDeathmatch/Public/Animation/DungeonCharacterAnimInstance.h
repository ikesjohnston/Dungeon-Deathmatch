// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "AnimGlobals.h"
#include "PlayerCharacter.h"

#include "DungeonCharacterAnimInstance.generated.h"

class UBlendSpace;
class UBlendSpace1D;
class UAnimSequence;

class ADungeonCharacter;

UCLASS(Blueprintable, BlueprintType)
class UDungeonCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ADungeonCharacter* OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	UCharacterMovementComponent* OwningCharacterMovement;

	/* The magnitude of the character's current velocity, used for movement blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementSpeed;

	/* The direction the player is moving, ranges from -180 to 180 degrees, used for movement blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementDirection;

	/* The amount of time, in seconds, that the owning character has been falling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float FallTime;

	/* Is the owning character crouching? Used for animation state transitions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsCrouched;

	/* Is the owning character jumping? Used for animation state transitions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsJumping;

	/* Is the owning character falling? Used for animation state transitions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsFalling;

	/* Is the owning character free looking? Used for animation state transitions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsFreeLooking;

	/* Is an animation montage playing for the owning character? Used for determining which animation slots to blend. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsMontagePlaying;

	/* Is the owning character turning? Used to determine when to play in place turning animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsReorientingBody;

	/* Should aim offsets be blended for the next animation update? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bShouldBlendAimOffset;

	/* Should the left turning animation be played during the turning animation state? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bShouldPlayLeftTurnAnimation;

	/* The yaw of the delta between the control rotation and character rotation, target value to lerp to, used for aim offset blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimYawTarget;

	/* The actual yaw value being plugged into animations. Is lerped towards AimYawTarget. */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float AimYaw;

	/* The pitch of the delta between the control rotation and character rotation, used for aim offset blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimPitch;

	/* The threshold the delta value of yaw targets between frames must exceed to trigger a aim lerp */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimYawDeltaLerpStart;

	bool bIsLerpingAim;

	/* The threshold the delta value of yaw targets between frames must be under to stop an ongoing aim lerp */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimYawDeltaLerpStop;

	/* The alpha value to use for lerp calculations to aim targets .*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimLerpAlpha;

	/* The blend space currently being used for standing movement animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* StandingMovementBlendSpace;

	/* The blend space previously used for standing movement animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* PreviousStandingMovementBlendSpace;

	/* Did the blend space used for standing movement animations change on the last tick? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bDidStandingMovementBlendSpaceChange;

	/* The blend space currently being used for crouching movement animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* CrouchingMovementBlendSpace;

	/* The blend space previously used for crouching movement animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* PreviousCrouchingMovementBlendSpace;

	/* Did the blend space used for crouching movement animations change on the last tick? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bDidCrouchingMovementBlendSpaceChange;

	/* The animation sequence currently being used for jumping animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UAnimSequence* JumpingAnimation;

	/* The animation sequence previously used for jumping animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UAnimSequence* PreviousJumpingAnimation;

	/* Did the animation sequence used for jumping animations change on the last tick? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bDidJumpAnimationChange;

	/* The blend space currently being used for falling animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace1D* FallingBlendSpace;

	/* The blend space previously used for falling animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace1D* PreviousFallingBlendSpace;

	/* Did the blend space used for falling animations change on the last tick? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bDidFallingBlendSpaceChange;

	/* The blend space currently being used for falling animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* LandingBlendSpace;

	/* The blend space previously used for falling animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	UBlendSpace* PreviousLandingBlendSpace;

	/* Did the blend space used for landing animations change on the last tick? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bDidLandingBlendSpaceChange;

	/* The time, in seconds, to blend to a new blend space when it changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float BlendSpaceChangeBlendTime;

	/* Is an animation montage currently being played in the default slot? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAnimPlayingInDefaultSlot;

	/* Is an animation montage currently being played in the upper body slot? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAnimPlayingInUpperBodySlot;

	/* Is an animation montage currently being played in the main hand slot? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAnimPlayingInMainHandSlot;

	/* Is an animation montage currently being played in the off hand slot? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAnimPlayingInOffHandSlot;

	/* Mapping of the number of animation montages being played for each slot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<EAnimSlot, int32> SlotAnimsBeingPlayed;

private:
	/** Timer handles and functions for reseting animation change flags, so that additional changes can be blended */
	FTimerHandle ResetStandingBlendSpaceChangeHandle;
	FTimerHandle ResetCrouchingBlendSpaceChangeHandle;
	FTimerHandle ResetJumpingAnimationChangeHandle;
	FTimerHandle ResetFallingBlendSpaceChangeHandle;
	FTimerHandle ResetLandingBlendSpaceChangeHandle;

	void ResetStandingBlendSpaceChange();
	void ResetCrouchingBlendSpaceChange();
	void ResetJumpingAnimationChange();
	void ResetFallingBlendSpaceChange();
	void ResetLandingBlendSpaceChange();

public:
	UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void UpdateAnimationResources();

	void SetMontageStartedInSlot(EAnimSlot Slot);

	void SetMontageStoppedInSlot(EAnimSlot Slot);
};