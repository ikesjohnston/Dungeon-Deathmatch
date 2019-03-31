// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_CheckDistance.h"
#include "DungeonCharacter.h"
#include "AIGlobals.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyAllTypes.h>

UBTDecorator_CheckDistance::UBTDecorator_CheckDistance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Accept only actors and vectors for target
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistance, Target), AActor::StaticClass());
	Target.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CheckDistance, Target));
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	bool Result = false;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR));

	if (Character)
	{
		FVector TargetLocation;
		AActor* Actor = Cast<AActor>(BlackboardComponent->GetValueAsObject(Target.SelectedKeyName));
		if (Actor)
		{
			TargetLocation = Actor->GetActorLocation();
		}
		else
		{
			TargetLocation = BlackboardComponent->GetValueAsVector(Target.SelectedKeyName);
		}

		float Distance = FVector::Distance(Character->GetActorLocation(), TargetLocation);
		bool WithinMinRange = (Distance >= DistanceMinimum || DistanceMinimum < 0);
		bool WithinMaxRange = (Distance <= DistanceMaximum || DistanceMaximum < 0);
		Result = (WithinMinRange && WithinMaxRange);
	}

	return Result;
}

FString UBTDecorator_CheckDistance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Distance to %s >= %.2f & <= %.2f"), *Target.SelectedKeyName.ToString(), DistanceMinimum, DistanceMaximum);
}
