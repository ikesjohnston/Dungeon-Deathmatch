// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Much of this code is derived from Epic's Action RPG example project that utilized the
 * Gameplay Abilities system.
 */

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
	// Constructor and overrides
	UDungeonDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
