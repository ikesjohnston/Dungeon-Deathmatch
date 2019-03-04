// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonSaveGame.h"

bool UDungeonSaveGame::IsMasterVolumeEnabled()
{
	return bIsMasterVolumeEnabled;
}

void UDungeonSaveGame::SetIsMasterVolumeEnabled(bool IsEnabled)
{
	bIsMasterVolumeEnabled = IsEnabled;
}

float UDungeonSaveGame::GetMasterVolume()
{
	return MasterVolume;
}

void UDungeonSaveGame::SetMasterVolume(float Volume)
{
	MasterVolume = Volume;
}

bool UDungeonSaveGame::IsEffectsVolumeEnabled()
{
	return bIsEffectsVolumeEnabled;
}

void UDungeonSaveGame::SetIsEffectsVolumeEnabled(bool IsEnabled)
{
	bIsEffectsVolumeEnabled = IsEnabled;
}

float UDungeonSaveGame::GetEffectsVolume()
{
	return EffectsVolume;
}

void UDungeonSaveGame::SetEffectsVolume(float Volume)
{
	EffectsVolume = Volume;
}

bool UDungeonSaveGame::IsUIVolumeEnabled()
{
	return bIsUIVolumeEnabled;
}

void UDungeonSaveGame::SetIsUIVolumeEnabled(bool IsEnabled)
{
	bIsUIVolumeEnabled = IsEnabled;
}

float UDungeonSaveGame::GetUIVolume()
{
	return UIVolume;
}

void UDungeonSaveGame::SetUIVolume(float Volume)
{
	UIVolume = Volume;
}

bool UDungeonSaveGame::IsMusicVolumeEnabled()
{
	return bIsMusicVolumeEnabled;
}

void UDungeonSaveGame::SetIsMusicVolumeEnabled(bool IsEnabled)
{
	bIsMusicVolumeEnabled = IsEnabled;
}

float UDungeonSaveGame::GetMusicVolume()
{
	return MusicVolume;
}

void UDungeonSaveGame::SetMusicVolume(float Volume)
{
	MusicVolume = Volume;
}
