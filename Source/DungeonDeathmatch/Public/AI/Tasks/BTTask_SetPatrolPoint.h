// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "BTTask_SetPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UBTTask_SetPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	virtual FString GetStaticDescription() const override;

};
