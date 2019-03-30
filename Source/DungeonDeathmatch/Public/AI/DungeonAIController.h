// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Perception/AIPerceptionTypes.h>

#include "DungeonAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UENUM(BlueprintType)
enum class EPatrolState : uint8
{
	PatrolArrivedAtNewLocation		UMETA(DisplayName = "Patrol Arrived At New Location"),
	PatrolMovingNewLocation			UMETA(DisplayName = "Patrol Moving To New Location"),
};

UCLASS()
class DUNGEONDEATHMATCH_API ADungeonAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Sight")
	float AISightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Sight")
	float AISightAge = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Sight")
	float AILoseSightRadius = AISightRadius + 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Sight")
	float AIFieldOfView = 60.0f;

	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Hearing")
	float AIHearingRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Perception|Hearing")
	float AIHearingAge = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName PatrolStatusKeyName = FName("PatrolStatus");
	uint8 PatrolStatusID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName PatrolTargetKeyName = FName("PatrolTarget");
	uint8 PatrolTargetKeyID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName SightTargetKeyName = FName("SightTarget");
	uint8 SightTargetKeyID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName HearingTargetKeyName = FName("HearingTarget");
	uint8 HearingTargetKeyID;

public:
	ADungeonAIController();

protected:
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void Possess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual FRotator GetControlRotation() const override;

protected:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
};
