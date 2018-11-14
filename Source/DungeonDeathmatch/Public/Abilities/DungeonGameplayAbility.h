// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Much of this code is derived from Epic's Action Dungeon example project that utilized the
 * Gameplay Abilities system.
 */

#pragma once

#include "GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Abilities/DungeonAbilityTypes.h"
#include "DungeonGameplayAbility.generated.h"

 /**
  * Subclass of ability blueprint type with game-specific data
  * This class uses GameplayEffectContainers to allow easier execution of gameplay effects based on a triggering tag
  * Most games will need to implement a subclass to support their game-specific code
  */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UDungeonGameplayAbility(const FObjectInitializer& ObjectInitializer);

	/** Map of gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FDungeonGameplayEffectContainer> EffectContainerMap;

	/** Make gameplay effect container spec to be applied later, using the passed in container */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FDungeonGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FDungeonGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FDungeonGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Applies a gameplay effect container spec that was previously created */
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FDungeonGameplayEffectContainerSpec& ContainerSpec);

	/** Applies a gameplay effect container, by creating and then applying the spec */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

};
