// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonAIController.h"
#include "DungeonDeathmatch.h"
#include "DungeonCharacter.h"
#include "PlayerCharacter.h"
#include "AIGlobals.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyAllTypes.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AISenseConfig_Hearing.h>
#include <TimerManager.h>

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

void ADungeonAIController::ClearStimuli()
{
	PrimarySightStimulus = FAIStimulus();
	PrimaryHearingStimulus = FAIStimulus();
}

void ADungeonAIController::StartPatrol()
{
	SetAIState(EAIState::Patrol);

	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_PATROLSTATE, (uint8) EAIPatrolState::PatrolStart);
}

void ADungeonAIController::SetAIState(EAIState State)
{
	AIState = State;
	if (State != EAIState::Combat)
	{
		BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_COMBATTARGET, nullptr);
	}
	ClearStimuli();
	BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_AISTATE, (uint8)AIState);
	UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::SetAIState - AI State for %s set to %s"), *GetPawn()->GetName(), *GetEnumValueAsString<EAIState>(TEXT("EAIState"), AIState));
	
}

void ADungeonAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	APlayerCharacter* CombatTarget = Cast<APlayerCharacter>(BlackboardComponent->GetValueAsObject(BLACKBOARD_KEYNAME_COMBATTARGET));
	AIState = (EAIState) BlackboardComponent->GetValueAsEnum(BLACKBOARD_KEYNAME_AISTATE);
	EAICombatState CombatState = (EAICombatState) BlackboardComponent->GetValueAsEnum(BLACKBOARD_KEYNAME_COMBATSTATE);
	for (AActor* Actor : UpdatedActors)
	{
		if(Actor == GetPawn()) continue;

		if (AIState == EAIState::Combat)
		{
			if (CombatTarget && Actor == CombatTarget)
			{
				bool WasVisible = bIsTargetVisible;
				bIsTargetVisible = false;
				FActorPerceptionBlueprintInfo PerceptionInfo;
				GetPerceptionComponent()->GetActorsPerception(Actor, PerceptionInfo);
				for (FAIStimulus Stimulus : PerceptionInfo.LastSensedStimuli)
				{
					if (Stimulus.IsActive())
					{
						if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
						{
							bIsTargetVisible = true;
							if (Stimulus.Strength > PrimarySightStimulus.Strength)
							{
								PrimarySightStimulus = Stimulus;
								UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s's primary sight stimulus location changed to %s."), *GetPawn()->GetName(), *PrimarySightStimulus.StimulusLocation.ToString());
							}
						}
						else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
						{
							if (Stimulus.Strength > PrimaryHearingStimulus.Strength)
							{
								PrimaryHearingStimulus = Stimulus;
								UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s's primary hearing stimulus location changed to %s."), *GetPawn()->GetName(), *PrimaryHearingStimulus.StimulusLocation.ToString());
							}
						}
					}	
				}
				if (bIsTargetVisible)
				{
					if (CombatState == EAICombatState::TargetLost)
					{
						UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s regained visual on target, now repositioning towards it."), *GetPawn()->GetName());
						BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_COMBATSTATE, (uint8)EAICombatState::Repositioning);
					}
					GetWorld()->GetTimerManager().ClearTimer(TargetLostHandle);
				}
				else if(WasVisible)
				{
					FVector InvestigateLocation = PrimarySightStimulus.Strength >= PrimaryHearingStimulus.Strength ? PrimarySightStimulus.StimulusLocation : PrimaryHearingStimulus.StimulusLocation;
					BlackboardComponent->SetValueAsVector(BLACKBOARD_KEYNAME_INVESTIGATELOCATION, InvestigateLocation);
					BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_COMBATSTATE, (uint8)EAICombatState::TargetLost);
					UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s lost visual on target. Moving to last known stimulus location %s."), *GetPawn()->GetName(), *InvestigateLocation.ToString());
					FTimerManager& TimerManager = GetWorld()->GetTimerManager();
					if (!TimerManager.IsTimerActive(TargetLostHandle))
					{
						TimerManager.SetTimer(TargetLostHandle, this, &ADungeonAIController::OnTargetLost, TargetLostEndCombatTimer, false);
					}		
				}
			}
		}
		else
		{
			FActorPerceptionBlueprintInfo PerceptionInfo;
			GetPerceptionComponent()->GetActorsPerception(Actor, PerceptionInfo);
			for (FAIStimulus Stimulus : PerceptionInfo.LastSensedStimuli)
			{
				if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
				{
					if (Stimulus.IsActive() && Stimulus.Strength >= PrimaryHearingStimulus.Strength)
					{
						PrimaryHearingStimulus = Stimulus;
						UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s heard a sound at %s."), *GetPawn()->GetName(), *Stimulus.StimulusLocation.ToString());
						FVector NoiseLocation = Stimulus.StimulusLocation;
						BlackboardComponent->SetValueAsVector(BLACKBOARD_KEYNAME_INVESTIGATELOCATION, NoiseLocation);
						if (AIState != EAIState::Combat)
						{
							UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s investigating new noise at %s."), *GetPawn()->GetName(), *NoiseLocation.ToString());
							SetAIState(EAIState::Investigate);
							BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_INVESTIGATESTATE, (uint8)EAIInvestigateState::HeardNoise);
						}
					}
				}
				else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
				{
					if (Stimulus.IsActive())
					{
						if (Stimulus.Strength >= PrimarySightStimulus.Strength)
						{
							PrimarySightStimulus = Stimulus;
						}
						if (AIState != EAIState::Combat)
						{
							UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s saw something at %s."), *GetPawn()->GetName(), *Stimulus.StimulusLocation.ToString(), Stimulus.IsExpired());
							APlayerCharacter* SightedPlayer = Stimulus.IsExpired() ? nullptr : Cast<APlayerCharacter>(Actor);
							BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_COMBATTARGET, SightedPlayer);
							UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnPerceptionUpdated - %s initiating combat."), *GetPawn()->GetName());
							SetAIState(EAIState::Combat);
							BlackboardComponent->SetValueAsEnum(BLACKBOARD_KEYNAME_COMBATSTATE, (uint8)EAICombatState::CombatStart);
						}	
					}
				}
			}
		}
	}
}

void ADungeonAIController::OnTargetLost()
{
	UE_LOG(LogTemp, Warning, TEXT("ADungeonAIController::OnTargetLost - %s has lost target. Returning to patrol."), *GetPawn()->GetName());
	BlackboardComponent->SetValueAsObject(BLACKBOARD_KEYNAME_COMBATTARGET, nullptr);
	SetAIState(EAIState::Patrol);
}
