// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonLobbyGameMode.h"
#include "DungeonGameInstance.h"

#include <Engine/World.h>
#include <TimerManager.h>

ADungeonLobbyGameMode::ADungeonLobbyGameMode()
{
	GameStartDelay = 10.0f;

	PlayerPercentageToStart = .75f;
}

void ADungeonLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumberOfPlayers++;
	UE_LOG(LogTemp, Warning, TEXT("ADungeonLobbyGameMode::PostLogin - %s joined the game, player count is %d"), *NewPlayer->GetName(), NumberOfPlayers);

	UDungeonGameInstance* GameInstance = Cast< UDungeonGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		float JoinedPercentage = (float) NumberOfPlayers / (float) GameInstance->GetMaxSessionConnections();
		UWorld* World = GetWorld();
		if (World && JoinedPercentage >= PlayerPercentageToStart)
		{
			World->GetTimerManager().SetTimer(GameStartTimer, this, &ADungeonLobbyGameMode::StartGame, GameStartDelay);
			GameInstance->BeginGameCountdown(GameStartDelay);
		}

		GameInstance->OnJoinedLobby();
	}
}

void ADungeonLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumberOfPlayers--;
	UE_LOG(LogTemp, Warning, TEXT("ADungeonLobbyGameMode::Logout - %s left the game, player count is %d"), *Exiting->GetName(), NumberOfPlayers);

	UDungeonGameInstance* GameInstance = Cast< UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnLeftLobby();
	}
}

void ADungeonLobbyGameMode::StartGame()
{
	UDungeonGameInstance* GameInstance = Cast< UDungeonGameInstance>(GetGameInstance());
	UWorld* World = GetWorld();

	if (GameInstance && World)
	{
		GameInstance->StartSession();
		bUseSeamlessTravel = true;
		FString MapString = GameInstance->GetSessionMapString();
		World->ServerTravel(MapString);
	}
}
