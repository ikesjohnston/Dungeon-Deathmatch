// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_Patrol.h"
#include "AIGlobals.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

void UBTService_Patrol::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	EAIPatrolState PatrolState = (EAIPatrolState) BlackboardComponent->GetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE);
	AAIPatrolPoint* PatrolPoint = Cast<AAIPatrolPoint>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_PATROLPOINT));
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR));
	if (Character)
	{
		if (PatrolPoint && PatrolState == EAIPatrolState::MovingToNewPatrolPoint)
		{
			float Distance = FVector::Distance(Character->GetActorLocation(), PatrolPoint->GetActorLocation());
			if (Distance <= PatrolPointArrivalDistance)
			{
				BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE, (uint8) EAIPatrolState::ArrivedAtNextPatrolPoint);
			}
		}
	}
}
