// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonTargetType.h"
#include "DungeonDeathmatch.h"
#include "DungeonGameplayAbility.h"
#include "Characters/DungeonCharacter.h"

void UDungeonTargetType::GetTargets_Implementation(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UDungeonTargetType_UseOwner::GetTargets_Implementation(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void UDungeonTargetType_UseEventData::GetTargets_Implementation(ADungeonCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}
}
