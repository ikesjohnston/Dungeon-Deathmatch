// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

#include <ConstructorHelpers.h>
#include <UserWidget.h>
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>

#include "MainMenuWidget.h"
#include "InGameMenuWidget.h"
#include "DungeonMenuWidget.h"
#include "LobbyWidget.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClass(TEXT("/Game/UI/Menus/WBP_MainMenu"));
	if (MainMenuClass.Class != NULL)
	{
		MainMenuWidgetClass = MainMenuClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClass(TEXT("/Game/UI/Menus/WBP_InGameMenu"));
	if (InGameMenuClass.Class != NULL)
	{
		InGameMenuWidgetClass = InGameMenuClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyClass(TEXT("/Game/UI/Menus/WBP_LobbyDisplay"));
	if (LobbyClass.Class != NULL)
	{
		LobbyWidgetClass = LobbyClass.Class;
	}
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
}

void UDungeonGameInstance::LoadMainMenu()
{
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UDungeonMenuWidget>(this, MainMenuWidgetClass);
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
		InGameMenuWidget = CreateWidget<UDungeonMenuWidget>(this, InGameMenuWidgetClass);
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
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
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

void UDungeonGameInstance::HostGame(FHostGameSettings Settings)
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
		PlayerController->ConsoleCommand("quit");
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
		TArray<FServerData> ServerData;
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			FServerData Server;
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

		UMainMenuWidget* MainMenu = Cast<UMainMenuWidget>(MainMenuWidget);
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
