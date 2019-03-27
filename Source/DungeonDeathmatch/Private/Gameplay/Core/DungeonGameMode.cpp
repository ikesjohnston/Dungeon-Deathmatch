// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameMode.h"
#include "DungeonCharacter.h"
#include "DungeonPlayerController.h"
#include "DungeonGameState.h"
#include "DungeonPlayerState.h"
#include "DungeonHUD.h"

#include <ConstructorHelpers.h>


ADungeonGameMode::ADungeonGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnBPClass(TEXT("/Game/Characters/Player/BP_PlayerCharacter"));
	if (DefaultPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = DefaultPawnBPClass.Class;
	}
	else
	{
		DefaultPawnClass = ADungeonCharacter::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Gameplay/Core/BP_DungeonPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	else
	{
		PlayerControllerClass = ADungeonPlayerController::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(TEXT("/Game/Gameplay/Core/BP_DungeonGameState"));
	if (GameStateBPClass.Class != NULL)
	{
		GameStateClass = GameStateBPClass.Class;
	}
	else
	{
		GameStateClass = ADungeonGameState::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<APlayerState> PlayerStateBPClass(TEXT("/Game/Gameplay/Core/BP_DungeonPlayerState"));
	if (PlayerStateBPClass.Class != NULL)
	{
		PlayerStateClass = PlayerStateBPClass.Class;
	}
	else
	{
		PlayerStateClass = ADungeonPlayerState::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/Gameplay/Core/BP_DungeonHUD"));
	if (HUDBPClass.Class != NULL)
	{
		HUDClass = HUDBPClass.Class;
	}
	else
	{
		HUDClass = ADungeonHUD::StaticClass();
	}
}

void ADungeonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADungeonGameMode::StartPlay()
{
	Super::StartPlay();
}
