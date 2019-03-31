// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonAIController.h"
#include "DungeonCharacter.h"
#include "PlayerCharacter.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyAllTypes.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AISenseConfig_Hearing.h>
#include "AIGlobals.h"

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

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = AIHearingRange;
	HearingConfig->SetMaxAge(AIHearingAge);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ADungeonAIController::OnPerceptionUpdated);
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
	else if (PropertyChangedEvent.GetPropertyName() == "AIHearingRange")
	{
		HearingConfig->HearingRange = AIHearingRange;
	}
	else if (PropertyChangedEvent.GetPropertyName() == "AIHearingAge")
	{
		HearingConfig->SetMaxAge(AIHearingAge);
	}
}

void ADungeonAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ADungeonCharacter* Character = Cast<ADungeonCharacter>(InPawn);
	if (Character && Character->GetBehaviorTree())
	{
		BlackboardComponent->InitializeBlackboard(*Character->GetBehaviorTree()->BlackboardAsset);
		SelfActorKeyID = BlackboardComponent->GetKeyID(BLACKBOARD_KEYNAME_SELFACTOR);
		BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_SELFACTOR, Character);
		AIStateKeyID = BlackboardComponent->GetKeyID(BLACKBOARD_KEYNAME_AISTATE);
		PatrolStateKeyID = BlackboardComponent->GetKeyID(BLACKBOARD_KEYNAME_PATROLSTATE);
		PatrolPointKeyID = BlackboardComponent->GetKeyID(BLACKBOARD_KEYNAME_PATROLPOINT);
		StartPatrol();
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

void ADungeonAIController::StartPatrol()
{
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_AISTATE, (uint8) EAIState::Patrol);
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE, (uint8) EAIPatrolState::PatrolStart);
}

void ADungeonAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	/*APlayerCharacter* SightedTarget = Cast<APlayerCharacter>(BlackboardComponent->GetValueAsObject(SightTargetKeyName));
	FVector HeardTargetLocation = BlackboardComponent->GetValueAsVector(HearingTargetKeyName);
	for (AActor* Actor : UpdatedActors)
	{
		if (SightedTarget && Actor == SightedTarget)
		{
			FActorPerceptionBlueprintInfo PerceptionInfo;
			GetPerceptionComponent()->GetActorsPerception(Actor, PerceptionInfo);
			for (FAIStimulus Stimulus : PerceptionInfo.LastSensedStimuli)
			{
			}
		}

		FActorPerceptionBlueprintInfo PerceptionInfo;
		GetPerceptionComponent()->GetActorsPerception(Actor, PerceptionInfo);
		for (FAIStimulus Stimulus : PerceptionInfo.LastSensedStimuli)
		{
			if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s heard a sound at %s. Expired: %d"), *GetPawn()->GetName(), *Stimulus.StimulusLocation.ToString(), Stimulus.IsExpired());
				FVector SoundLocation = Stimulus.IsExpired() ? FVector::ZeroVector : Stimulus.StimulusLocation;
				BlackboardComponent->SetValueAsVector(HearingTargetKeyName, SoundLocation);
			}
			if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s saw something at %s. Expired: %d"), *GetPawn()->GetName(), *Stimulus.StimulusLocation.ToString(), Stimulus.IsExpired());
				APlayerCharacter* TargetPlayer = Stimulus.IsExpired() ? nullptr : Cast<APlayerCharacter>(Actor);
				BlackboardComponent->SetValueAsObject(SightTargetKeyName, TargetPlayer);
			}
		}
	}*/
}
