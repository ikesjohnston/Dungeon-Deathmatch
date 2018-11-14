// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonAbilitySystemComponent.h"
#include "DungeonDeathmatch.h"
#include "Characters/DungeonCharacter.h"
#include "DungeonGameplayAbility.h"
#include "AbilitySystemGlobals.h"

UDungeonAbilitySystemComponent::UDungeonAbilitySystemComponent() {}

void UDungeonAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UDungeonGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<UDungeonGameplayAbility>(ActiveAbility));
		}
	}
}

int32 UDungeonAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ADungeonCharacter* OwningCharacter = Cast<ADungeonCharacter>(OwnerActor);

	if (OwningCharacter)
	{
		//return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

UDungeonAbilitySystemComponent* UDungeonAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UDungeonAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
