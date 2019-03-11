// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GMSNetworkGlobals.h"
#include "GMSSettingsGlobals.h"
#include "GMSMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGMSMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for the implementation of high level, game wide menu functions
 */
class GAMEMENUSYSTEM_API IGMSMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HostGame(FGMSHostGameSettings Settings) = 0;

	virtual void JoinGame(uint32 Index) = 0;

	virtual void BeginGameCountdown(float Delay) = 0;

	virtual void ExitToMainMenu() = 0;

	virtual void ExitToDesktop() = 0;

	virtual void RefreshServerList() = 0;

	virtual TMap<FString, FString> GetGameModes() = 0;

	virtual TMap<FString, FString> GetMaps() = 0;

	virtual FGMSAudioSettings GetAudioSettings() = 0;

	virtual void SetAudioSettings(FGMSAudioSettings Settings, bool ApplyImmediately = true) = 0;
};
