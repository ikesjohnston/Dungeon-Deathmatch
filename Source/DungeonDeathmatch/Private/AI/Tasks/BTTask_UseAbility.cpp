// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_UseAbility.h"
#include "AIGlobals.h"
#include "DungeonCharacter.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <AbilitySystemComponent.h>

EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR));
	if (Character)
	{
		//Character->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(FGameplayAbilitySpec(Ability, Character->GetCharacterLevel(), INDEX_NONE, Character));
		Character->GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability, Character->GetCharacterLevel(), INDEX_NONE, Character));
		Character->GetAbilitySystemComponent()->TryActivateAbilityByClass(Ability);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
