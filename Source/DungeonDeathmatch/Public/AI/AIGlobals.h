// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define BLACKBOARD_KEYNAME_SELFACTOR	FName("SelfActor")
#define BLACKBOARD_KEYNAME_AISTATE		FName("AIState")
#define BLACKBOARD_KEYNAME_PATROLSTATE	FName("PatrolState")
#define BLACKBOARD_KEYNAME_PATROLPOINT	FName("PatrolPoint")

UENUM(BlueprintType, meta = (DisplayName = "AI State"))
enum class EAIState : uint8
{
	Patrol			UMETA(DisplayName = "Patrol"),
	Investigate		UMETA(DisplayName = "Investigate"),
	Combat			UMETA(DisplayName = "Combat"),
};

UENUM(BlueprintType, meta = (DisplayName = "AI Patrol State"))
enum class EAIPatrolState : uint8
{
	PatrolStart					UMETA(DisplayName = "Patrol Start"),
	MovingToNewPatrolPoint		UMETA(DisplayName = "Moving To New Patrol Point"),
	ArrivedAtNextPatrolPoint	UMETA(DisplayName = "Arrived At Next Patrol Point"),
};

UENUM(BlueprintType, meta = (DisplayName = "AI Investigate State"))
enum class EAIInvestigateState : uint8
{
	InvestigateStart				UMETA(DisplayName = "Investigate Start"),
	MovingToLocation		UMETA(DisplayName = "Moving To Location"),
	SearchingLocation		UMETA(DisplayName = "Searching Location"),
};

UENUM(BlueprintType, meta = (DisplayName = "AI Combat State"))
enum class EAICombatState : uint8
{
	CombatStart			UMETA(DisplayName = "Combat Start"),
	Attacking			UMETA(DisplayName = "Attacking"),
	Repositioning		UMETA(DisplayName = "Repositioning"),
};