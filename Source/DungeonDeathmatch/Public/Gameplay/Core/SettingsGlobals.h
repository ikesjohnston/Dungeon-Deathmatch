#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerInput.h>
#include "SettingsGlobals.generated.h"

/**
 * Struct for storing primary and secondary key binding strings
 */
USTRUCT()
struct FDungeonKeyBind
{
	GENERATED_BODY()

	FString ActionName;
	FInputActionKeyMapping PrimaryBinding;
	FInputActionKeyMapping SecondaryBinding;
};

/**
 * Struct for storing audio settings
 */
USTRUCT()
struct FDungeonAudioSettings
{
	GENERATED_BODY()

	bool MasterVolumeEnabled;
	float MasterVolume;
	bool EffectsVolumeEnabled;
	float EffectsVolume;
	bool UIVolumeEnabled;
	float UIVolume;
	bool MusicVolumeEnabled;
	float MusicVolume;
};