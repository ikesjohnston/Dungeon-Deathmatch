// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Much of this code is derived from Epic's Action Dungeon example project that utilized the
 * Gameplay Abilities system.
 */

 // ----------------------------------------------------------------------------------------------------------------
 // This header is for Ability-specific structures and enums that are shared across a project
 // Every game will probably need a file like this to handle their extensions to the system
 // This file is a good place for subclasses of FGameplayEffectContext and FGameplayAbilityTargetData
 // ----------------------------------------------------------------------------------------------------------------

#pragma once

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "DungeonAbilityTypes.generated.h"

class UDungeonAbilitySystemComponent;
class UGameplayEffect;
class UDungeonTargetType;


/**
 * Struct defining a list of gameplay effects, a tag, and targeting info
 * These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 */
USTRUCT(BlueprintType)
struct FDungeonGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FDungeonGameplayEffectContainer() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<UDungeonTargetType> TargetType;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/** A "processed" version of DungeonGameplayEffectContainer that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct FDungeonGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FDungeonGameplayEffectContainerSpec() {}

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};
