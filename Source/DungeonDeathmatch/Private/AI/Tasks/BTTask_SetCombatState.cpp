// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetCombatState.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetCombatState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_COMBATSTATE, (uint8) CombatState);

	return EBTNodeResult::Succeeded;
}
