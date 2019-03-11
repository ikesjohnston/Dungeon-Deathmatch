// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

#include <ConstructorHelpers.h>
#include <UserWidget.h>
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>

#include "DungeonSaveGame.h"
#include "GMSMenuWidgetBase.h"
#include "GMSMainMenuWidget.h"
#include "GMSInGameMenuWidget.h"
#include "GMSLobbyWidget.h"

const static int32 DEFAULT_MAX_PLAYERS = 2;
const static FName SESSION_NAME					= TEXT("My Game Session");

const static FName SESSION_SETTING_GAME_NAME	= TEXT("GameName");
const static FName SESSION_SETTING_GAME_MODE	= TEXT("GameMode");
const static FName SESSION_SETTING_GAME_SIZE	= TEXT("GameSize");
const static FName SESSION_SETTING_MAP			= TEXT("Map");

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridSlotSize = 40.0f;
}

void UDungeonGameInstance::Init()
{
	if (MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Found main menu class %s"), *MainMenuWidgetClass->GetName());
	}

	if (InGameMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Found in game menu class %s"), *InGameMenuWidgetClass->GetName());
	}

	if (LobbyWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Foundlobby display widget class %s"), *LobbyWidgetClass->GetName());
	}

	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Found online subsystem %s"), *OSS->GetSubsystemName().ToString());
		SessionInterface = OSS->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDungeonGameInstance::OnCreateSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDungeonGameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UDungeonGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDungeonGameInstance::OnFindSessionsComplete);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - No session interface found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - No online subsystem found"));
	}

	LoadGameSettings();
	ApplyAudioSettings();

	UGameUserSettings* UserSettings = const_cast<UGameUserSettings*>(GetDefault<UGameUserSettings>());
	if (!UserSettings) return;
	UserSettings->ApplySettings(true);
}

void UDungeonGameInstance::LoadMainMenu()
{
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UGMSMenuWidgetBase>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->SetMenuInterface(this);
			MainMenuWidget->Setup();
		}
	}
}

void UDungeonGameInstance::LoadInGameMenu()
{
	if (InGameMenuWidgetClass)
	{
		InGameMenuWidget = CreateWidget<UGMSMenuWidgetBase>(this, InGameMenuWidgetClass);
		if (InGameMenuWidget)
		{
			InGameMenuWidget->SetMenuInterface(this);
			InGameMenuWidget->Setup();
		}
	}
}

void UDungeonGameInstance::LoadLobbyDisplay()
{
	if (LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<UGMSLobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidget)
		{
			FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
			if (ExistingSession)
			{
				LobbyWidget->Setup(ExistingSession->SessionSettings.NumPublicConnections);
			}
			LobbyWidget->AddToViewport();
		}
	}
}

void UDungeonGameInstance::HostGame(FGMSHostGameSettings Settings)
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}

		DesiredHostGameSettings = Settings;
		CreateSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::HostGame - No session interface found"));
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("No session interface found")));
		}
	}
}

void UDungeonGameInstance::JoinGame(uint32 Index)
{
	if (SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		if (MainMenuWidget)
		{
			MainMenuWidget->Teardown();
		}

		SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::JoinGame - No session interface found"));
	}
}

void UDungeonGameInstance::OnJoinedLobby()
{
	MulticastOnJoinedLobby();
}

void UDungeonGameInstance::MulticastOnJoinedLobby_Implementation()
{
	if (LobbyWidget)
	{
		LobbyWidget->OnPlayerJoined();
	}
}

void UDungeonGameInstance::OnLeftLobby()
{
	MulticastOnJoinedLobby();
}

void UDungeonGameInstance::MulticastOnLeftLobby_Implementation()
{
	if (LobbyWidget)
	{
		LobbyWidget->OnPlayerLeft();
	}
}

void UDungeonGameInstance::BeginGameCountdown(float Delay)
{
	MulticastBeginGameCountdown(Delay);
}

void UDungeonGameInstance::MulticastBeginGameCountdown_Implementation(float Delay)
{
	if (LobbyWidget)
	{
		LobbyWidget->BeginCountdown(Delay);
	}
}

void UDungeonGameInstance::ExitToMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ClientTravel("/Game/Levels/MainMenuMap", ETravelType::TRAVEL_Absolute);
	}
}

void UDungeonGameInstance::ExitToDesktop()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("Exit");
	}
}

void UDungeonGameInstance::RefreshServerList()
{
	if (SessionInterface.IsValid())
	{
		if (SessionSearch.IsValid())
		{
			SessionSearch->MaxSearchResults = 1000000; // Only needed for shared AppID
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::RefreshServerList - Started session search"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::RefreshServerList - No session interface found"));
	}
}

void UDungeonGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SessionName);
	}
}

int32 UDungeonGameInstance::GetMaxSessionConnections()
{
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
	if (ExistingSession)
	{
		return ExistingSession->SessionSettings.NumPublicConnections;
	}

	return 0;
}

FString UDungeonGameInstance::GetSessionMapString()
{
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
	if (ExistingSession)
	{
		FString MapKey;
		FString GameModeKey;
		ExistingSession->SessionSettings.Get(SESSION_SETTING_MAP, MapKey);
		ExistingSession->SessionSettings.Get(SESSION_SETTING_GAME_MODE, GameModeKey);
		if (!MapKey.IsEmpty())
		{
			FString* MapStringPtr = Maps.Find(MapKey);
			if (MapStringPtr)
			{
				FString MapString = *MapStringPtr;
				if (!GameModeKey.IsEmpty())
				{
					FString* GameModeStringPtr = GameModes.Find(GameModeKey);
					if (GameModeStringPtr)
					{
						FString GameModeString = *GameModeStringPtr;
						FString CompleteMapString = FString::Printf(TEXT("%s?game=%s"), *MapString, *GameModeString);
						return CompleteMapString;
					}
				}
				return *MapStringPtr;
			}
		}
	}
	return FString(TEXT(""));
}

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
}

TMap<FString, FString> UDungeonGameInstance::GetGameModes()
{
	return GameModes;
}

TMap<FString, FString> UDungeonGameInstance::GetMaps()
{
	return Maps;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierColors()
{
	return ItemQualityTierColors;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierTextColors()
{
	return ItemQualityTierTextColors;
}

TSubclassOf<UDraggableItemWidget> UDungeonGameInstance::GetDragAndDropItemWidgetClass()
{
	return DragAndDropItemWidgetClass;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetSheatheAnimationMontages()
{
	return SheatheAnimationMontages;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetUnsheatheAnimationMontages()
{
	return UnsheatheAnimationMontages;
}

float UDungeonGameInstance::GetInventoryGridSlotSize()
{
	return InventoryGridSlotSize;
}

FVector UDungeonGameInstance::GetRandomLootEjectionForce()
{
	// Get a random forward ejection vector
	FVector EjectionVector = FVector(LootEjectionForwardForce, 0, LootEjectionUpForce);
	float RandomForwardAngle = FMath::FRand() * 360.0f;
	EjectionVector.RotateAngleAxis(RandomForwardAngle, FVector::UpVector);

	return EjectionVector;
}

void UDungeonGameInstance::LoadGameSettings()
{
	GameSettings =  Cast<UDungeonSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0));
	if (!GameSettings)
	{
		GameSettings = Cast<UDungeonSaveGame>(UDungeonSaveGame::StaticClass()->GetDefaultObject());
		GameSettings->SetIsMasterVolumeEnabled(DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME_ENABLED);
		GameSettings->SetMasterVolume(DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME);
		GameSettings->SetIsEffectsVolumeEnabled(DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME_ENABLED);
		GameSettings->SetEffectsVolume(DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME);
		GameSettings->SetIsUIVolumeEnabled(DEFAULT_SETTINGS_AUDIO_UI_VOLUME_ENABLED);
		GameSettings->SetUIVolume(DEFAULT_SETTINGS_AUDIO_UI_VOLUME);
		GameSettings->SetIsMusicVolumeEnabled(DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME_ENABLED);
		GameSettings->SetMusicVolume(DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME);
	}
}

void UDungeonGameInstance::SaveGameSettings()
{
	if (GameSettings)
	{
		UGameplayStatics::SaveGameToSlot(GameSettings, SaveGameSlotName, 0);
	}
}

FGMSAudioSettings UDungeonGameInstance::GetAudioSettings()
{
	FGMSAudioSettings Settings;
	Settings.MasterVolume = DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME;
	Settings.EffectsVolume = DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME;
	Settings.UIVolume = DEFAULT_SETTINGS_AUDIO_UI_VOLUME;
	Settings.MusicVolume = DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME;

	if (GameSettings)
	{
		Settings.MasterVolumeEnabled = GameSettings->IsMasterVolumeEnabled();
		Settings.MasterVolume = GameSettings->GetMasterVolume();
		Settings.EffectsVolumeEnabled = GameSettings->IsEffectsVolumeEnabled();
		Settings.EffectsVolume = GameSettings->GetEffectsVolume();
		Settings.UIVolumeEnabled = GameSettings->IsUIVolumeEnabled();
		Settings.UIVolume = GameSettings->GetUIVolume();
		Settings.MusicVolumeEnabled = GameSettings->IsMusicVolumeEnabled();
		Settings.MusicVolume = GameSettings->GetMusicVolume();
	}

	return Settings;
}

void UDungeonGameInstance::SetAudioSettings(FGMSAudioSettings Settings, bool ApplyImmediately /*=true*/)
{
	if (GameSettings)
	{
		GameSettings->SetIsMasterVolumeEnabled(Settings.MasterVolumeEnabled);
		GameSettings->SetMasterVolume(Settings.MasterVolume);
		GameSettings->SetIsEffectsVolumeEnabled(Settings.EffectsVolumeEnabled);
		GameSettings->SetEffectsVolume(Settings.EffectsVolume);
		GameSettings->SetIsUIVolumeEnabled(Settings.UIVolumeEnabled);
		GameSettings->SetUIVolume(Settings.UIVolume);
		GameSettings->SetIsMusicVolumeEnabled(Settings.MusicVolumeEnabled);
		GameSettings->SetMusicVolume(Settings.MusicVolume);
		SaveGameSettings();
	}
	if (ApplyImmediately)
	{
		ApplyAudioSettings();
	}
}

void UDungeonGameInstance::ApplyAudioSettings()
{
	float MasterVolume = DEFAULT_SETTINGS_AUDIO_MASTER_VOLUME;
	float EffectsVolume = DEFAULT_SETTINGS_AUDIO_EFFECTS_VOLUME;
	float UIVolume = DEFAULT_SETTINGS_AUDIO_UI_VOLUME;
	float MusicVolume = DEFAULT_SETTINGS_AUDIO_MUSIC_VOLUME;

	if (GameSettings)
	{
		if (GameSettings->IsMasterVolumeEnabled())
		{
			MasterVolume = GameSettings->GetMasterVolume();
		}
		else
		{
			MasterVolume = 0.0f;
		}
		if (GameSettings->IsEffectsVolumeEnabled())
		{
			EffectsVolume = GameSettings->GetEffectsVolume();
		}
		else
		{
			EffectsVolume = 0.0f;
		}
		if (GameSettings->IsUIVolumeEnabled())
		{
			UIVolume = GameSettings->GetUIVolume();
		}
		else
		{
			UIVolume = 0.0f;
		}
		if (GameSettings->IsMusicVolumeEnabled())
		{
			MusicVolume = GameSettings->GetMusicVolume();
		}
		else
		{
			MusicVolume = 0.0f;
		}
	}

	if (SoundMixClass)
	{
		if (MasterSoundClass)
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixClass, MasterSoundClass, MasterVolume, 1.0f, 0.0f);
		}
		if (EffectsSoundClass)
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixClass, EffectsSoundClass, EffectsVolume, 1.0f, 0.0f);
		}
		if (UISoundClass)
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixClass, UISoundClass, UIVolume, 1.0f, 0.0f);
		}
		if (MusicSoundClass)
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixClass, MusicSoundClass, MusicVolume, 1.0f, 0.0f);
		}
	}
}

void UDungeonGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		SessionSettings.NumPublicConnections = 2;

		EGameSize* GameSizePtr = GameSizes.Find(DesiredHostGameSettings.GameSize);
		if (GameSizePtr)
		{
			EGameSize GameSize = *GameSizePtr;
			int32* PlayerCountPtr = GameSizePlayerCounts.Find(GameSize);
			if (PlayerCountPtr)
			{
				SessionSettings.NumPublicConnections = *PlayerCountPtr;
			}
		}

		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SESSION_SETTING_GAME_NAME, DesiredHostGameSettings.Name, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SESSION_SETTING_GAME_MODE, DesiredHostGameSettings.GameModeString, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SESSION_SETTING_MAP, DesiredHostGameSettings.MapString, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
			SessionName = NAME_GameSession;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
			SessionName = SESSION_NAME;
		}
		SessionInterface->CreateSession(0, SessionName, SessionSettings);
	}
}

void UDungeonGameInstance::OnCreateSessionComplete(FName SessionName, bool WasSessionCreated)
{
	if (WasSessionCreated)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnCreateSessionComplete - Created online session %s"), *SessionName.ToString());
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Created online session %s"), *SessionName.ToString()));
		}

		if (MainMenuWidget)
		{
			MainMenuWidget->Teardown();
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/Levels/LobbyMap?listen");
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnCreateSessionComplete - Failed to create online session %s"), *SessionName.ToString());
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Failed to create online session %s"), *SessionName.ToString()));
		}
	}
}

void UDungeonGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success && SessionInterface && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnJoinSessionComplete - Joined session %s, traveling to level..."), *SessionName.ToString());
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joined session %s, traveling to level..."), *SessionName.ToString()));
		}

		FString ConnectInfo;
		if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
		{
			UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnJoinSessionComplete - Couldn't not resolve connection string"));
			return;
		}

		APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
		if (PlayerController)
		{
			PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnJoinSessionComplete - Couldn't travel to level, no player controller found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnJoinSessionComplete - Failed to join session"));
	}
}

void UDungeonGameInstance::OnDestroySessionComplete(FName SessionName, bool WasSessionDestroyed)
{
	if (WasSessionDestroyed)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnDestroySessionComplete - Destroyed online session %s"), *SessionName.ToString());
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Destroyed online session %s"), *SessionName.ToString()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnDestroySessionComplete - Failed to destroy online session %s"), *SessionName.ToString());
		UEngine* Engine = GetEngine();
		if (Engine)
		{
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Failed to create online session %s"), *SessionName.ToString()));
		}
	}
}

void UDungeonGameInstance::OnFindSessionsComplete(bool WasSearchSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnFindSessionsComplete - Stopped session search"));
	if (WasSearchSuccessful && SessionSearch.IsValid() && MainMenuWidget)
	{
		TArray<FGMSServerData> ServerData;
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			FGMSServerData Server;
			Server.HostUsername = Result.Session.OwningUserName;
			Server.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Server.CurrentPlayers = Server.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Server.Latency = Result.PingInMs;

			FString GameName;
			if (Result.Session.SessionSettings.Get(SESSION_SETTING_GAME_NAME, GameName))
			{
				Server.Name = GameName;
				UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnFindSessionsComplete - Found game %s"), *Server.Name);
			}
			else
			{
				Server.Name = Result.GetSessionIdStr();
				UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnFindSessionsComplete - No game name found for session %s"), *Server.Name);
			}

			ServerData.Add(Server);
		}

		UGMSMainMenuWidget* MainMenu = Cast<UGMSMainMenuWidget>(MainMenuWidget);
		if (MainMenu)
		{
			MainMenu->PopulateServerList(ServerData);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnFindSessionsComplete - No sessions found"));
	}
}