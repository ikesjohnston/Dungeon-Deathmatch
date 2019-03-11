// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GMSLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMENUSYSTEM_API UGMSLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UWidget* PlayerCountDisplay;

	UPROPERTY(meta = (BindWidget))
	UWidget* GameCountdownDisplay;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	bool GameStarting;

	UPROPERTY(BlueprintReadWrite)
	float GameStartDelay;

public:
	void Setup(int32 MaxPlayerCount);

	void OnPlayerJoined();

	void OnPlayerLeft();

	void BeginCountdown(float Delay);
};
