// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "BTService_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UBTService_Patrol : public UBTService
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float PatrolPointArrivalDistance;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
