// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "MoveSpeedChangeExecution.generated.h"

/**
 * A speed change execution, which will calculate the correct value to set a character's movement speed to 
 * based on the defined walking, sprinting, or crouching speeds of that character.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UMoveSpeedChangeExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMoveSpeedChangeExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	
};
