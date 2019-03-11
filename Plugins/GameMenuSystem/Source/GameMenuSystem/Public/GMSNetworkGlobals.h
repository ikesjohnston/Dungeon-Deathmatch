#pragma once

#include "CoreMinimal.h"
#include "GMSNetworkGlobals.generated.h"

/**
 * Struct for storing properties of a discovered server
 */
USTRUCT()
struct FGMSServerData
{
	GENERATED_BODY()

	FString Name;
	uint8 CurrentPlayers;
	uint8 MaxPlayers;
	FString HostUsername;
	int32 Latency;
};

/**
 * Struct for storing desired settings for a game to be hosted
 */
USTRUCT()
struct FGMSHostGameSettings
{
	GENERATED_BODY()

	FString Name;
	FString GameSize;
	FString GameModeString;
	FString MapString;
};