// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameMode.h"
#include "DungeonGameState.h"
#include "DungeonPlayerState.h"
#include "DungeonCharacter.h"
#include "DungeonHUD.h"


ADungeonGameMode::ADungeonGameMode()
{
	DefaultPawnClass = ADungeonCharacter::StaticClass();
	GameStateClass = ADungeonGameState::StaticClass();
	PlayerStateClass = ADungeonPlayerState::StaticClass();
	HUDClass = ADungeonHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ADungeonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADungeonGameMode::GameOver()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}

void ADungeonGameMode::StartPlay()
{
	Super::StartPlay();
}
