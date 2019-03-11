// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSInGameMenuWidget.h"
#include "GMSMenuInterface.h"
#include "GMSSettingsMenuWidget.h"

#include <Button.h>
#include <WidgetSwitcher.h>

bool UGMSInGameMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(ResumeButton != nullptr))
	{
		return false;
	}
	ResumeButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnResumeButtonPressed);

	if (!ensure(SettingsButton != nullptr))
	{
		return false;
	}
	SettingsButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnSettingsButtonPressed);
	
	if (!ensure(SettingsMenu != nullptr))
	{
		return false;
	}
	SettingsMenu->OnBackButtonClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnSettingsMenuBackButtonPressed);

	if (!ensure(LeaveGameButton != nullptr))
	{
		return false;
	}
	LeaveGameButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnLeaveGameButtonPressed);

	if (!ensure(LeaveGameAcceptButton != nullptr))
	{
		return false;
	}
	LeaveGameAcceptButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnLeaveGameAcceptButtonPressed);

	if (!ensure(LeaveGameCancelButton != nullptr))
	{
		return false;
	}
	LeaveGameCancelButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnLeaveGameCancelButtonPressed);

	if (!ensure(ExitButton != nullptr))
	{
		return false;
	}
	ExitButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnExitButtonPressed);

	if (!ensure(ExitAcceptButton != nullptr))
	{
		return false;
	}
	ExitAcceptButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnExitAcceptButtonPressed);

	if (!ensure(ExitCancelButton != nullptr))
	{
		return false;
	}
	ExitCancelButton->OnClicked.AddDynamic(this, &UGMSInGameMenuWidget::OnExitCancelButtonPressed);

	return Result;
}

void UGMSInGameMenuWidget::SetMenuInterface(IGMSMenuInterface* MenuInterface)
{
	Super::SetMenuInterface(MenuInterface);

	if (SettingsMenu)
	{
		SettingsMenu->SetMenuInterface(MenuInterface);
	}
}

void UGMSInGameMenuWidget::OnResumeButtonPressed()
{
	Teardown();
}

void UGMSInGameMenuWidget::OnSettingsButtonPressed()
{
	MenuSwitcher->SetActiveWidget(SettingsMenu);
}

void UGMSInGameMenuWidget::OnSettingsMenuBackButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

void UGMSInGameMenuWidget::OnLeaveGameButtonPressed()
{
	MenuSwitcher->SetActiveWidget(ConfirmLeaveGameMenu);
}

void UGMSInGameMenuWidget::OnLeaveGameAcceptButtonPressed()
{
	if (MenuInterface)
	{
		MenuInterface->ExitToMainMenu();
	}
}

void UGMSInGameMenuWidget::OnLeaveGameCancelButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

void UGMSInGameMenuWidget::OnExitButtonPressed()
{
	MenuSwitcher->SetActiveWidget(ConfirmExitMenu);
}

void UGMSInGameMenuWidget::OnExitAcceptButtonPressed()
{
	if (MenuInterface)
	{
		MenuInterface->ExitToDesktop();
	}
}

void UGMSInGameMenuWidget::OnExitCancelButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

FReply UGMSInGameMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FReply Reply = FReply::Unhandled();

	if (MenuSwitcher && InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (MenuSwitcher->GetActiveWidget() == InGameMenu)
		{
			Teardown();
		}
		else
		{
			MenuSwitcher->SetActiveWidget(InGameMenu);
		}
		Reply = FReply::Handled();
	}

	return Reply;
}
