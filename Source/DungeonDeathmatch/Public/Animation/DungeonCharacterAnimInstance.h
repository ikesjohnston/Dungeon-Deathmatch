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

	/* The delta rotation yaw from the control rotation to the character rotation, used for aim offset blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimYaw;

	/* The delta rotation pitch from the control rotation to the character rotation, used for aim offset blendspaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	float AimPitch;

public:
	UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	UBlendSpace* GetMovementBlendSpace();
};