// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include <Button.h>
#include "MenuInterface.h"
#include <WidgetSwitcher.h>
#include <EditableTextBox.h>

bool UMainMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!MainMenuHostButton || !MainMenuJoinButton || !JoinMenuJoinButton || !JoinMenuBackButton)
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
	}
}

void UMainMenuWidget::OnJoinMenuJoinButtonPressed()
{
	if (MenuInterface && IPAddressField)
	{
		MenuInterface->JoinGame(IPAddressField->GetText().ToString());
	}
}

void UMainMenuWidget::OnJoinMenuBackButtonPressed()
{
	if (MenuSwitcher)
	{
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
