// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

#include <ConstructorHelpers.h>
#include <UserWidget.h>
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>

#include "MainMenuWidget.h"
#include "InGameMenuWidget.h"
#include "DungeonMenuWidget.h"

const static FName SESSION_NAME = TEXT("My Game Session");

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridSlotSize = 40.0f;

	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClass(TEXT("/Game/UI/Menus/WBP_MainMenu"));
	MainMenuWidgetClass = MainMenuClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClass(TEXT("/Game/UI/Menus/WBP_InGameMenu"));
	InGameMenuWidgetClass = InGameMenuClass.Class;
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

void UDungeonGameInstance::HostGame()
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
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

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
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

		SessionSettings.bIsLANMatch = false;
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
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
			World->ServerTravel("/Game/Levels/TraversalTestMap?listen");
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
			Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Created online session %s"), *SessionName.ToString()));
		}
		CreateSession();
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
	if (WasSearchSuccessful && SessionSearch.IsValid())
	{
		UMainMenuWidget* MainMenu = Cast<UMainMenuWidget>(MainMenuWidget);
		if (MainMenu)
		{
			MainMenu->PopulateServerList(SessionSearch->SearchResults);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::OnFindSessionsComplete - No sessions found"));
	}
}
