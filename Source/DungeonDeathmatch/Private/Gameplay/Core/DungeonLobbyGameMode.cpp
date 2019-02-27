// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonLobbyGameMode.h"
#include "DungeonGameInstance.h"

#include <Engine/World.h>
#include <TimerManager.h>

ADungeonLobbyGameMode::ADungeonLobbyGameMode()
{
	GameStartDelay = 10.0f;
}

void ADungeonLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumberOfPlayers++;
	UE_LOG(LogTemp, Warning, TEXT("ADungeonLobbyGameMode::PostLogin - %s joined the game, player count is %d"), *NewPlayer->GetName(), NumberOfPlayers);

	UWorld* World = GetWorld();
	if (World && NumberOfPlayers >= 2)
	{
		World->GetTimerManager().SetTimer(GameStartTimer, this, &ADungeonLobbyGameMode::StartGame, GameStartDelay);
	}
}

void ADungeonLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumberOfPlayers--;
	UE_LOG(LogTemp, Warning, TEXT("ADungeonLobbyGameMode::Logout - %s left the game, player count is %d"), *Exiting->GetName(), NumberOfPlayers);
}

void ADungeonLobbyGameMode::StartGame()
{
	UDungeonGameInstance* GameInstance = Cast< UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->StartSession();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		World->ServerTravel("/Game/Levels/TraversalTestMap?listen");
	}
}
