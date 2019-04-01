// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetAIState.h"
#include "DungeonAIController.h"

#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/BlackboardComponent.h>

EBTNodeResult::Type UBTTask_SetAIState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_AISTATE, (uint8) AIState);

	AActor* Pawn = Cast<AActor>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR));
	if (Pawn)
	{
		ADungeonAIController* Controller = Cast<ADungeonAIController>(Pawn->GetInstigatorController());
		if (Controller)
		{
			if (AIState != EAIState::Combat)
			{
				BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_COMBATTARGET, nullptr);
			}
			Controller->ClearStimuli();
		}
		UE_LOG(LogTemp, Warning, TEXT("EBTNodeResult::Type UBTTask_SetAIState - AI State for %s set to %s"), *Pawn->GetName(), *GetEnumValueAsString<EAIState>(TEXT("EAIState"), AIState));
	}

	return EBTNodeResult::Succeeded;
}
