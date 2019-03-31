// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "AIGlobals.h"
#include "BTTask_SetAIState.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UBTTask_SetAIState : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	EAIState AIState;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
