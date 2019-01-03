// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "DungeonDamageExecution.generated.h"

/**
 * A damage execution, which allows doing damage by combining a raw Damage number with AttackPower and DefensePower
 * Most games will want to implement multiple game-specific executions
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDungeonDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
