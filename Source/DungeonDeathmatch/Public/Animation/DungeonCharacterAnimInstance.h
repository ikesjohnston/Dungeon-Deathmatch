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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	float MovementDirection;

public:
	UDungeonCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	UBlendSpace* GetMovementBlendSpace();

	UFUNCTION(BlueprintPure)
	UBlendSpace* GetJumpBlendSpace();

	UFUNCTION(BlueprintPure)
	UBlendSpace* GetLandBlendSpace();
};