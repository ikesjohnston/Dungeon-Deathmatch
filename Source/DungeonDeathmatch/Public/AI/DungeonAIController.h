// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "DungeonAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAISenseConfig_Sight;

UCLASS()
class DUNGEONDEATHMATCH_API ADungeonAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float AISightRadius = 500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float AISightAge = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float AILoseSightRadius = AISightRadius + 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception|Sight")
	float AIFieldOfView = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName TargetKeyName = FName("Target");
	uint8 TargetKeyID;

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
	void OnPawnDetected(const TArray<AActor*>& DetectedPawns);
};
