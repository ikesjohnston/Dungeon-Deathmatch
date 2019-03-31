// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetPatrolState.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetPatrolState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE, (uint8) PatrolState);

	return EBTNodeResult::Succeeded;
}