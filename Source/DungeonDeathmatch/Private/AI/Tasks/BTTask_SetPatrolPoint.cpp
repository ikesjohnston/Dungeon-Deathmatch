// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetPatrolPoint.h"
#include "AIGlobals.h"
#include "DungeonCharacter.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR));
	if (Character)
	{
		TArray<AAIPatrolPoint*> PatrolPoints = Character->GetPatrolPoints();
		if (PatrolPoints.Num() > 1)
		{
			AAIPatrolPoint* CurrentPatrolPoint = Cast<AAIPatrolPoint>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_PATROLPOINT));
			AAIPatrolPoint* NextPatrolPoint = CurrentPatrolPoint;

			while (NextPatrolPoint == CurrentPatrolPoint)
			{
				NextPatrolPoint = PatrolPoints[FMath::RandRange(0, PatrolPoints.Num() - 1)];
			}
			BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_PATROLPOINT, NextPatrolPoint);
			BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE, (uint8) EAIPatrolState::MovingToNewPatrolPoint);
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_SetPatrolPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("Get new patrol point."));
}
