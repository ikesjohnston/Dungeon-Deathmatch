// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetInvestigateState.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetInvestigateState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_INVESTIGATESTATE, (uint8) InvestigateState);

	return EBTNodeResult::Succeeded;
}
