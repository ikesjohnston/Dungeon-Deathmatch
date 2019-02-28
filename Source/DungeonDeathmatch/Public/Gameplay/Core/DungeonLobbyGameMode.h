// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Core/DungeonGameMode.h"
#include "DungeonLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonLobbyGameMode : public ADungeonGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GameStartDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayerPercentageToStart;

private:
	uint32 NumberOfPlayers;

	FTimerHandle GameStartTimer;

public:
	ADungeonLobbyGameMode();

protected:
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	void StartGame();
};
