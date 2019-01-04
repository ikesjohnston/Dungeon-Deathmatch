// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "DungeonCharacter.h"
#include "DungeonCharacterAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UDungeonCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ADungeonCharacter* OwningCharacter;

	/* The magnitude of the character's current velocity, used for movement blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementSpeed;

	/* The direction the player is moving, ranges from -180 to 180 degrees, used for movement blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementDirection;

	/* Used to determine when to play in place turning animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	bool bIsReorientingBody;

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

public:
	UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	UBlendSpace* GetMovementBlendSpace();
};