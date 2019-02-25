// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include <Button.h>
#include "MenuInterface.h"
#include <WidgetSwitcher.h>
#include <EditableTextBox.h>
#include <ConstructorHelpers.h>
#include "ServerBrowserLineWidget.h"
#include <TextBlock.h>
#include <OnlineSessionSettings.h>

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerDetailsClass(TEXT("/Game/UI/Menus/WBP_ServerBrowserLine"));
	ServerDetailsWidgetClass = ServerDetailsClass.Class;
}

bool UMainMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(ServerDetailsWidgetClass != nullptr))
	{
		return false;
	}

	if (!ensure(ServerListRefreshDisplay != nullptr))
	{
		return false;
	}

	if (!ensure(MainMenuHostButton != nullptr))
	{
		return false;
	}
	MainMenuHostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMainMenuHostButtonPressed);

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
	if (MenuInterface)
	{
		MenuInterface->HostGame();
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
		ServerList->SetVisibility(ESlateVisibility::Collapsed);
		ServerListRefreshDisplay->SetVisibility(ESlateVisibility::Visible);
		MenuInterface->RefreshServerList();
	}
}

void UMainMenuWidget::PopulateServerList(TArray<FOnlineSessionSearchResult> SearchResults)
{
	ServerList->ClearChildren();
	ServerList->SetVisibility(ESlateVisibility::Visible);
	ServerListRefreshDisplay->SetVisibility(ESlateVisibility::Collapsed);

	UWorld* World = GetWorld();
	if (World)
	{
		uint32 ServerIndex = 0;
		for (const FOnlineSessionSearchResult& Result : SearchResults)
		{
			UServerBrowserLineWidget* ServerDetailsLine = CreateWidget<UServerBrowserLineWidget>(World, ServerDetailsWidgetClass);
			if (ServerDetailsLine)
			{
				ServerDetailsLine->SetSessionNameText(FText::FromString(Result.GetSessionIdStr()));
				ServerDetailsLine->SetPlayerCountText(FText::FromString(FString::FromInt(Result.Session.NumOpenPublicConnections)));
				ServerDetailsLine->SetLatencyText(FText::FromString(FString::Printf(TEXT("%d ms"), Result.PingInMs)));
				ServerDetailsLine->Setup(this, ServerIndex);
				ServerIndex++;

				ServerList->AddChild(ServerDetailsLine);
			}
		}
	}
}

void UMainMenuWidget::SelectServerIndex(uint32 Index)
{
	SelectedServerIndex = Index;
	JoinMenuJoinButton->SetIsEnabled(true);
}

