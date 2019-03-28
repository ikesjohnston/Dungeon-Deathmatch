// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonAIController.h"
#include "DungeonCharacter.h"
#include "PlayerCharacter.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyAllTypes.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AIPerceptionComponent.h>

ADungeonAIController::ADungeonAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ADungeonAIController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void ADungeonAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("All AI systems set."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Errors initializing AI systems."));
	}
}

void ADungeonAIController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == "AISightRadius")
	{
		SightConfig->SightRadius = AISightRadius;
	}
	else if (PropertyChangedEvent.GetPropertyName() == "AILoseSightRadius")
	{
		SightConfig->LoseSightRadius = AILoseSightRadius;
	}
	else if (PropertyChangedEvent.GetPropertyName() == "AIFieldOfView")
	{
		SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	}
	else if (PropertyChangedEvent.GetPropertyName() == "AISightAge")
	{
		SightConfig->SetMaxAge(AISightAge);
	}
}

void ADungeonAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ADungeonCharacter* Character = Cast<ADungeonCharacter>(InPawn);
	if (Character && Character->GetBehaviorTree())
	{
		BlackboardComponent->InitializeBlackboard(*Character->GetBehaviorTree()->BlackboardAsset);
		TargetKeyID = BlackboardComponent->GetKeyID(TargetKeyName);
		BehaviorTreeComponent->StartTree(*Character->GetBehaviorTree());
	}
}

void ADungeonAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

FRotator ADungeonAIController::GetControlRotation() const
{
	if (!GetPawn())
	{
		return FRotator::ZeroRotator;
	}

	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void ADungeonAIController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	APlayerCharacter* TargetPlayer = nullptr;
	for (AActor* Actor : DetectedPawns)
	{
		TargetPlayer = Cast<APlayerCharacter>(Actor);
		if (TargetPlayer) break;
	}
	
	BlackboardComponent->SetValueAsObject(TargetKeyName, TargetPlayer);
}
