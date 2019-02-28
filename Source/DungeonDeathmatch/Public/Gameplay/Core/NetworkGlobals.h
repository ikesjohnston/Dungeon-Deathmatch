#pragma once

#include "CoreMinimal.h"
#include "NetworkGlobals.generated.h"

class ADungeonGameMode;

/**
 * Enum representation for game size. Used to determine the size of generated levels and max player count.
 */
UENUM(BlueprintType) enum class EGameSize : uint8 {
	Small		UMETA(DisplayName = "Small"),
	Medium		UMETA(DisplayName = "Medium"),
	Large		UMETA(DisplayName = "Large")
};

/**
 * Struct for storing properties of a discovered server
 */
USTRUCT()
struct FServerData
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
struct FHostGameSettings
{
	GENERATED_BODY()

	FString Name;
	FString GameSize;
	FString GameModeString;
	FString MapString;
};