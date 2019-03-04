// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DungeonSaveGame.generated.h"

const static bool	DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME_ENABLED	= true;
const static float	DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME			= 1.0f;
const static bool	DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME_ENABLED	= true;
const static float	DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME			= 1.0f;
const static bool	DEFAULT_SETTINGS_AUDIO_UI_VOLUME_ENABLED		= true;
const static float	DEFAULT_SETTINGS_AUDIO_UI_VOLUME				= 1.0f;
const static bool	DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME_ENABLED		= true;
const static float	DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME				= 1.0f;

/**
 * Save Game class used for saving miscellaneous settings
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonSaveGame : public USaveGame
{
	GENERATED_BODY()

protected:
	// ---------------------- BEGIN AUDIO SETTINGS VARIABLES --------------------------
	UPROPERTY()
	bool bIsMasterVolumeEnabled;

	UPROPERTY()
	float MasterVolume;

	UPROPERTY()
	bool bIsEffectsVolumeEnabled;

	UPROPERTY()
	float EffectsVolume;

	UPROPERTY()
	bool bIsUIVolumeEnabled;

	UPROPERTY()
	float UIVolume;

	UPROPERTY()
	bool bIsMusicVolumeEnabled;

	UPROPERTY()
	float MusicVolume;
	// ---------------------- END AUDIO SETTINGS VARIABLES --------------------------

public:
	// ---------------------- BEGIN AUDIO SETTINGS FUNCTIONS --------------------------
	UFUNCTION()
	bool IsMasterVolumeEnabled();

	UFUNCTION()
	void SetIsMasterVolumeEnabled(bool IsEnabled);

	UFUNCTION()
	float GetMasterVolume();

	UFUNCTION()
	void SetMasterVolume(float Volume);

	UFUNCTION()
	bool IsEffectsVolumeEnabled();

	UFUNCTION()
	void SetIsEffectsVolumeEnabled(bool IsEnabled);

	UFUNCTION()
	float GetEffectsVolume();

	UFUNCTION()
	void SetEffectsVolume(float Volume);

	UFUNCTION()
	bool IsUIVolumeEnabled();

	UFUNCTION()
	void SetIsUIVolumeEnabled(bool IsEnabled);

	UFUNCTION()
	float GetUIVolume();

	UFUNCTION()
	void SetUIVolume(float Volume);

	UFUNCTION()
	bool IsMusicVolumeEnabled();

	UFUNCTION()
	void SetIsMusicVolumeEnabled(bool IsEnabled);

	UFUNCTION()
	float GetMusicVolume();

	UFUNCTION()
	void SetMusicVolume(float Volume);
	// ---------------------- END AUDIO SETTINGS FUNCTIONS --------------------------
};
