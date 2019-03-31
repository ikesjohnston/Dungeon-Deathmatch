// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector Target;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float DistanceMinimum;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float DistanceMaximum;

public:
	UBTDecorator_CheckDistance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	virtual FString GetStaticDescription() const override;

};
