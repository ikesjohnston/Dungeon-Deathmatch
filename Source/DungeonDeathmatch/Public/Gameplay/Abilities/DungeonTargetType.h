// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Much of this code is derived from Epic's Action Dungeon example project that utilized the
 * Gameplay Abilities system.
 */

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/DungeonAbilityTypes.h"
#include "DungeonTargetType.generated.h"

class ADungeonCharacter;
class AActor;
struct FGameplayEventData;

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class DUNGEONDEATHMATCH_API UDungeonTargetType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UDungeonTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class DUNGEONDEATHMATCH_API UDungeonTargetType_UseOwner : public UDungeonTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UDungeonTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class DUNGEONDEATHMATCH_API UDungeonTargetType_UseEventData : public UDungeonTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UDungeonTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
