// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetAIState.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetAIState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_AISTATE, (uint8) AIState);

	return EBTNodeResult::Succeeded;
}
