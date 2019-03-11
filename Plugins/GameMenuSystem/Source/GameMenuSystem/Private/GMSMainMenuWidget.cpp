// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSMainMenuWidget.h"
#include "GMSMenuInterface.h"
#include "GMSSettingsMenuWidget.h"
#include "GMSServerBrowserRowWidget.h"

#include <Button.h>
#include <WidgetSwitcher.h>
#include <EditableTextBox.h>
#include <ConstructorHelpers.h>
#include <ComboBoxString.h>

UGMSMainMenuWidget::UGMSMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsMapDropdownInitialized = false;
	bIsGameModesDropdownInitialized = false;
}

void UGMSMainMenuWidget::SetMenuInterface(IGMSMenuInterface* MenuInterface)
{
	Super::SetMenuInterface(MenuInterface);

	if (SettingsMenu)
	{
		SettingsMenu->SetMenuInterface(MenuInterface);
	}
}

bool UGMSMainMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!InitializeHostMenu() || !InitializeJoinMenu() || !InitializeSettingsMenu() || !InitializeExitMenu())
	{
		Result = false;
	}

	return Result;
}

bool UGMSMainMenuWidget::InitializeHostMenu()
{
	if (!ensure(MainMenuHostButton != nullptr))
	{
		return false;
	}
	MainMenuHostButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnMainMenuHostButtonPressed);

	if (!ensure(HostMenuHostButton != nullptr))
	{
		return false;
	}
	HostMenuHostButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnHostMenuHostButtonPressed);

	if (!ensure(HostMenuBackButton != nullptr))
	{
		return false;
	}
	HostMenuBackButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnHostMenuBackButtonPressed);

	if (!ensure(HostMenuGameNameField != nullptr))
	{
		return false;
	}
	HostMenuGameNameField->OnTextChanged.AddDynamic(this, &UGMSMainMenuWidget::OnHostMenuGameNameFieldChanged);

	if (!ensure(GameTypeDropdown != nullptr))
	{
		return false;
	}
	GameTypeDropdown->OnSelectionChanged.AddDynamic(this, &UGMSMainMenuWidget::OnGameTypeSelectionChanged);

	if (!ensure(GameSizeDropdown != nullptr))
	{
		return false;
	}
	GameSizeDropdown->AddOption(FString("Small"));
	GameSizeDropdown->AddOption(FString("Medium"));
	GameSizeDropdown->AddOption(FString("Large"));
	GameSizeDropdown->SetSelectedOption(GameSizeDropdown->GetOptionAtIndex(0));
	GameSizeDropdown->OnSelectionChanged.AddDynamic(this, &UGMSMainMenuWidget::OnGameSizeSelectionChanged);

	if (!ensure(MapDropdown != nullptr))
	{
		return false;
	}
	MapDropdown->OnSelectionChanged.AddDynamic(this, &UGMSMainMenuWidget::OnMapSelectionChanged);

	return true;
}

bool UGMSMainMenuWidget::InitializeJoinMenu()
{
	if (!ensure(MainMenuJoinButton != nullptr))
	{
		return false;
	}
	MainMenuJoinButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnMainMenuJoinButtonPressed);

	if (!ensure(JoinMenuJoinButton != nullptr))
	{
		return false;
	}
	JoinMenuJoinButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnJoinMenuJoinButtonPressed);

	if (!ensure(JoinMenuRefreshButton != nullptr))
	{
		return false;
	}
	JoinMenuRefreshButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnJoinMenuRefreshButtonPressed);

	if (!ensure(JoinMenuBackButton != nullptr))
	{
		return false;
	}
	JoinMenuBackButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnJoinMenuBackButtonPressed);

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

	return true;
}

bool UGMSMainMenuWidget::InitializeSettingsMenu()
{
	if (!ensure(SettingsButton != nullptr))
	{
		return false;
	}
	SettingsButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnSettingsButtonPressed);

	if (!ensure(SettingsMenu != nullptr))
	{
		return false;
	}
	SettingsMenu->OnBackButtonClicked.AddDynamic(this, &UGMSMainMenuWidget::OnSettingsMenuBackButtonPressed);

	return true;
}

bool UGMSMainMenuWidget::InitializeExitMenu()
{
	if (!ensure(ExitButton != nullptr))
	{
		return false;
	}
	ExitButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnExitButtonPressed);

	if (!ensure(ExitAcceptButton != nullptr))
	{
		return false;
	}
	ExitAcceptButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnExitAcceptButtonPressed);

	if (!ensure(ExitCancelButton != nullptr))
	{
		return false;
	}
	ExitCancelButton->OnClicked.AddDynamic(this, &UGMSMainMenuWidget::OnExitCancelButtonPressed);

	return true;
}

void UGMSMainMenuWidget::OnMainMenuHostButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(HostMenu);

		if (MenuInterface)
		{
			InitializeMapDropdown();
			InitializeGameModesDropdown();
		}

		HostMenuHostButton->SetIsEnabled(false);
	}
}

void UGMSMainMenuWidget::InitializeGameModesDropdown()
{
	if (!bIsGameModesDropdownInitialized && GameTypeDropdown)
	{
		TMap<FString, FString> GameModes = MenuInterface->GetGameModes();
		for (TTuple<FString, FString> GameMode : GameModes)
		{
			GameTypeDropdown->AddOption(GameMode.Key);
		}
		if (GameTypeDropdown->GetOptionCount() > 0)
		{
			GameTypeDropdown->SetSelectedOption(GameTypeDropdown->GetOptionAtIndex(0));
		}
		bIsGameModesDropdownInitialized = true;
	}
}

void UGMSMainMenuWidget::InitializeMapDropdown()
{
	if (!bIsMapDropdownInitialized && MapDropdown)
	{
		TMap<FString, FString> Maps = MenuInterface->GetMaps();
		for (TTuple<FString, FString> Map : Maps)
		{
			MapDropdown->AddOption(Map.Key);
		}
		if (MapDropdown->GetOptionCount() > 0)
		{
			MapDropdown->SetSelectedOption(MapDropdown->GetOptionAtIndex(0));
		}
		bIsMapDropdownInitialized = true;
	}
}

void UGMSMainMenuWidget::OnHostMenuHostButtonPressed()
{
	if (MenuInterface && HostMenuGameNameField)
	{
		FGMSHostGameSettings Settings;
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

void UGMSMainMenuWidget::OnHostMenuBackButtonPressed()
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

void UGMSMainMenuWidget::OnMainMenuJoinButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
		RefreshServerList();
	}
}

void UGMSMainMenuWidget::OnJoinMenuJoinButtonPressed()
{
	if (SelectedServerIndex.IsSet() && MenuInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGMSMainMenuWidget::OnJoinMenuJoinButtonPressed - Selected server index %d"), SelectedServerIndex.GetValue());
		MenuInterface->JoinGame(SelectedServerIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGMSMainMenuWidget::OnJoinMenuJoinButtonPressed - Selected server index not set"));
	}
}

void UGMSMainMenuWidget::OnJoinMenuRefreshButtonPressed()
{
	RefreshServerList();
}

void UGMSMainMenuWidget::OnJoinMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
		SelectedServerIndex.Reset();
		JoinMenuJoinButton->SetIsEnabled(false);
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UGMSMainMenuWidget::OnSettingsButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(SettingsMenu);
	}
}

void UGMSMainMenuWidget::OnSettingsMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UGMSMainMenuWidget::OnExitButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(ConfirmExitMenu);
	}
}

void UGMSMainMenuWidget::OnExitAcceptButtonPressed()
{
	MenuInterface->ExitToDesktop();
}

void UGMSMainMenuWidget::OnExitCancelButtonPressed()
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UGMSMainMenuWidget::RefreshServerList()
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

void UGMSMainMenuWidget::PopulateServerList(TArray<FGMSServerData> SearchResults)
{
	ServerList->ClearChildren();
	ServerListRefreshDisplay->SetVisibility(ESlateVisibility::Collapsed);
	ServerListEmptyDisplay->SetVisibility(ESlateVisibility::Collapsed);

	UWorld* World = GetWorld();
	if (World)
	{
		uint32 ServerIndex = 0;
		for (const FGMSServerData& Result : SearchResults)
		{
			UGMSServerBrowserRowWidget* ServerDetailsLine = CreateWidget<UGMSServerBrowserRowWidget>(World, ServerDetailsWidgetClass);
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

void UGMSMainMenuWidget::SelectServerIndex(uint32 Index)
{
	SelectedServerIndex = Index;
	JoinMenuJoinButton->SetIsEnabled(true);

	UpdateServerRowWidgets();
}

void UGMSMainMenuWidget::UpdateServerRowWidgets()
{
	for (int32 ChildIndex = 0; ChildIndex < ServerList->GetChildrenCount(); ChildIndex++)
	{
		UGMSServerBrowserRowWidget* ServerRow = Cast<UGMSServerBrowserRowWidget>(ServerList->GetChildAt(ChildIndex));
		if (ServerRow)
		{
			ServerRow->Selected = (SelectedServerIndex.IsSet() && SelectedServerIndex.GetValue() == ChildIndex);
		}
	}
}

void UGMSMainMenuWidget::OnHostMenuGameNameFieldChanged(const FText& Text)
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

void UGMSMainMenuWidget::OnGameTypeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}

void UGMSMainMenuWidget::OnGameSizeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}

void UGMSMainMenuWidget::OnMapSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

}

FReply UGMSMainMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FReply Reply = FReply::Handled();

	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}

	return Reply;
}
