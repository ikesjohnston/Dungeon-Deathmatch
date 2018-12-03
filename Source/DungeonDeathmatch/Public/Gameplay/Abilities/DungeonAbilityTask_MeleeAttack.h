// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "DungeonAbilityTask_MeleeAttack.generated.h"

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDungeonMeleeAttackDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonAbilityTask_MeleeAttack : public UAbilityTask_PlayMontageAndWait
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UDungeonAbilityTask_MeleeAttack(const FObjectInitializer& ObjectInitializer);

	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FDungeonMeleeAttackDelegate EventReceived;

private:
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
};
