// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include "MenuInterface.h"
#include "ServerBrowserRowWidget.h"
#include "DungeonGameInstance.h"
#include "DungeonGameMode.h"

#include <Button.h>
#include <WidgetSwitcher.h>
#include <EditableTextBox.h>
#include <ConstructorHelpers.h>
#include <ComboBoxString.h>

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerDetailsClass(TEXT("/Game/UI/Menus/WBP_ServerBrowserRow"));
	if (ServerDetailsClass.Class != NULL)
	{
		ServerDetailsWidgetClass = ServerDetailsClass.Class;
	}
}

bool UMainMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(ServerDetailsWidgetClass != nullptr))
	{
		return false;
	}

	if (!ensure(ServerListDisplay != nullptr))
	{
		return false;
	}

	if (!ensure(ServerListRefreshDisplay != nullptr))
	{
		return false;
	}
	
	if (!ensure(ServerListEmptyDisplay != nullptr))
	{
		return false;
	}

	if (!ensure(MainMenuHostButton != nullptr))
	{
		return false;
	}
	MainMenuHostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMainMenuHostButtonPressed);

	if (!ensure(HostMenuHostButton != nullptr))
	{
		return false;
	}
	HostMenuHostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostMenuHostButtonPressed);

	if (!ensure(HostMenuBackButton != nullptr))
	{
		return false;
	}
	HostMenuBackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostMenuBackButtonPressed);

	if (!ensure(HostMenuGameNameField != nullptr))
	{
		return false;
	}
	HostMenuGameNameField->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnHostMenuGameNameFieldChanged);

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (!ensure(GameTypeDropdown != nullptr))
	{
		return false;
	}
	if (GameInstance)
	{
		TMap<FString, FString> GameModes = GameInstance->GetGameModes();
		for (TTuple<FString, FString> GameMode : GameModes)
		{
			GameTypeDropdown->AddOption(GameMode.Key);
		}
		if (GameTypeDropdown->GetOptionCount() > 0)
		{
			GameTypeDropdown->SetSelectedOption(GameTypeDropdown->GetOptionAtIndex(0));
		}
	}
	GameTypeDropdown->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnGameTypeSelectionChanged);

	if (!ensure(GameSizeDropdown != nullptr))
	{
		return false;
	}
	GameSizeDropdown->AddOption(FString("Small"));
	GameSizeDropdown->AddOption(FString("Medium"));
	GameSizeDropdown->AddOption(FString("Large"));
	GameSizeDropdown->SetSelectedOption(GameSizeDropdown->GetOptionAtIndex(0));
	GameSizeDropdown->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnGameSizeSelectionChanged);

	if (!ensure(MapDropdown != nullptr))
	{
		return false;
	}
	if (GameInstance)
	{
		TMap<FString, FString> Maps = GameInstance->GetMaps();
		for (TTuple<FString, FString> Map : Maps)
		{
			MapDropdown->AddOption(Map.Key);
		}
		if (MapDropdown->GetOptionCount() > 0)
		{
			MapDropdown->SetSelectedOption(MapDropdown->GetOptionAtIndex(0));
		}
	}
	MapDropdown->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnMapSelectionChanged);

	if (!ensure(MainMenuJoinButton != nullptr))
	{
		return false;
	}
	MainMenuJoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMainMenuJoinButtonPressed);

	if (!ensure(JoinMenuJoinButton != nullptr))
	{
		return false;
	}
	JoinMenuJoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinMenuJoinButtonPressed);

	if (!ensure(JoinMenuRefreshButton != nullptr))
	{
		return false;
	}
	JoinMenuRefreshButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinMenuRefreshButtonPressed);

	if (!ensure(JoinMenuBackButton != nullptr))
	{
		return false;
	}
	JoinMenuBackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinMenuBackButtonPressed);

	if (!ensure(SettingsButton != nullptr))
	{
		return false;
	}
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonPressed);

	if (!ensure(SettingsMenuBackButton != nullptr))
	{
		return false;
	}
	SettingsMenuBackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsMenuBackButtonPressed);

	if (!ensure(ExitButton != nullptr))
	{
		return false;
	}
	ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitButtonPressed);

	if (!ensure(ExitAcceptButton != nullptr))
	{
		return false;
	}
	ExitAcceptButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitAcceptButtonPressed);

	if (!ensure(ExitCancelButton != nullptr))
	{
		return false;
	}
	ExitCancelButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitCancelButtonPressed);

	return Result;
}

void UMainMenuWidget::OnMainMenuHostButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(HostMenu);
		HostMenuHostButton->SetIsEnabled(false);
	}
}

void UMainMenuWidget::OnHostMenuHostButtonPressed()
{
	if (MenuInterface && HostMenuGameNameField)
	{
		FHostGameSettings Settings;
		Settings.Name = HostMenuGameNameField->GetText().ToString();
		if (GameTypeDropdown)
		{
			Settings.GameModeString = GameTypeDropdown->GetSelectedOption();

		}
		if (GameSizeDropdown)
		{
			Settings.GameSize = GameSizeDropdown->GetSelectedOption();
		}
		if (MapDropdown)
		{
			Settings.MapString = MapDropdown->GetSelectedOption();
		}
		MenuInterface->HostGame(Settings);
	}
}

void UMainMenuWidget::OnHostMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
		if (HostMenuGameNameField)
		{
			HostMenuGameNameField->SetText(FText::GetEmpty());
		}
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenuWidget::OnMainMenuJoinButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
		RefreshServerList();
	}
}

void UMainMenuWidget::OnJoinMenuJoinButtonPressed()
{
	if (SelectedServerIndex.IsSet() && MenuInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnJoinMenuJoinButtonPressed - Selected server index %d"), SelectedServerIndex.GetValue());
		MenuInterface->JoinGame(SelectedServerIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnJoinMenuJoinButtonPressed - Selected server index not set"));
	}
}

void UMainMenuWidget::OnJoinMenuRefreshButtonPressed()
{
	RefreshServerList();
}

void UMainMenuWidget::OnJoinMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
		SelectedServerIndex.Reset();
		JoinMenuJoinButton->SetIsEnabled(false);
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenuWidget::OnSettingsButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(SettingsMenu);
	}
}

void UMainMenuWidget::OnSettingsMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenuWidget::OnExitButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(ConfirmExitMenu);
	}
}

void UMainMenuWidget::OnExitAcceptButtonPressed()
{
	MenuInterface->ExitToDesktop();
}

void UMainMenuWidget::OnExitCancelButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}


void UMainMenuWidget::RefreshServerList()
{
	if (MenuInterface)
	{
		SelectedServerIndex.Reset();
		JoinMenuJoinButton->SetIsEnabled(false);
		ServerListDisplay->SetVisibility(ESlateVisibility::Collapsed);
		ServerListEmptyDisplay->SetVisibility(ESlateVisibility::Collapsed);
		ServerListRefreshDisplay->SetVisibility(ESlateVisibility::Visible);
		MenuInterface->RefreshServerList();
	}
}

void UMainMenuWidget::PopulateServerList(TArray<FServerData> SearchResults)
{
	ServerList->ClearChildren();
	ServerListRefreshDisplay->SetVisibility(ESlateVisibility::Collapsed);
	ServerListEmptyDisplay->SetVisibility(ESlateVisibility::Collapsed);

	UWorld* World = GetWorld();
	if (World)
	{
		uint32 ServerIndex = 0;
		for (const FServerData& Result : SearchResults)
		{
			UServerBrowserRowWidget* ServerDetailsLine = CreateWidget<UServerBrowserRowWidget>(World, ServerDetailsWidgetClass);
			if (ServerDetailsLine)
			{
				ServerDetailsLine->SetSessionNameText(FText::FromString(Result.Name));
				ServerDetailsLine->SetHostNameText(FText::FromString(Result.HostUsername));
				ServerDetailsLine->SetPlayerCountText(FText::FromString(FString::Printf(TEXT("%d/%d"), Result.CurrentPlayers, Result.MaxPlayers)));
				ServerDetailsLine->SetLatencyText(FText::FromString(FString::Printf(TEXT("%d ms"), Result.Latency)));
				ServerDetailsLine->Setup(this, ServerIndex);
				ServerIndex++;

				ServerList->AddChild(ServerDetailsLine);
			}
		}

		if (ServerList->GetChildrenCount() > 0)
		{
			ServerListDisplay->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ServerListEmptyDisplay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UMainMenuWidget::SelectServerIndex(uint32 Index)
{
	SelectedServerIndex = Index;
	JoinMenuJoinButton->SetIsEnabled(true);

	UpdateServerRowWidgets();
}

void UMainMenuWidget::UpdateServerRowWidgets()
{
	for (int32 ChildIndex = 0; ChildIndex < ServerList->GetChildrenCount(); ChildIndex++)
	{
		UServerBrowserRowWidget* ServerRow = Cast<UServerBrowserRowWidget>(ServerList->GetChildAt(ChildIndex));
		if (ServerRow)
		{
			ServerRow->Selected = (SelectedServerIndex.IsSet() && SelectedServerIndex.GetValue() == ChildIndex);
		}
	}
}

void UMainMenuWidget::OnHostMenuGameNameFieldChanged(const FText& Text)
{
	if (Text.IsEmptyOrWhitespace())
	{
		// Can't host a game without a valid name
		HostMenuHostButton->SetIsEnabled(false);
	}
	else
	{
		HostMenuHostButton->SetIsEnabled(true);
	}
}

void UMainMenuWidget::OnGameTypeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}

void UMainMenuWidget::OnGameSizeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}

void UMainMenuWidget::OnMapSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}
