// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenuWidget.h"
#include "MenuInterface.h"
#include "SettingsMenuWidget.h"

#include <Button.h>
#include <WidgetSwitcher.h>

bool UInGameMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(ResumeButton != nullptr))
	{
		return false;
	}
	ResumeButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnResumeButtonPressed);

	if (!ensure(SettingsButton != nullptr))
	{
		return false;
	}
	SettingsButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnSettingsButtonPressed);
	
	if (!ensure(SettingsMenu != nullptr))
	{
		return false;
	}
	SettingsMenu->OnBackButtonClicked.AddDynamic(this, &UInGameMenuWidget::OnSettingsMenuBackButtonPressed);

	if (!ensure(LeaveGameButton != nullptr))
	{
		return false;
	}
	LeaveGameButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnLeaveGameButtonPressed);

	if (!ensure(LeaveGameAcceptButton != nullptr))
	{
		return false;
	}
	LeaveGameAcceptButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnLeaveGameAcceptButtonPressed);

	if (!ensure(LeaveGameCancelButton != nullptr))
	{
		return false;
	}
	LeaveGameCancelButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnLeaveGameCancelButtonPressed);

	if (!ensure(ExitButton != nullptr))
	{
		return false;
	}
	ExitButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnExitButtonPressed);

	if (!ensure(ExitAcceptButton != nullptr))
	{
		return false;
	}
	ExitAcceptButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnExitAcceptButtonPressed);

	if (!ensure(ExitCancelButton != nullptr))
	{
		return false;
	}
	ExitCancelButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnExitCancelButtonPressed);

	return Result;
}

void UInGameMenuWidget::OnResumeButtonPressed()
{
	Teardown();
}

void UInGameMenuWidget::OnSettingsButtonPressed()
{
	MenuSwitcher->SetActiveWidget(SettingsMenu);
}

void UInGameMenuWidget::OnSettingsMenuBackButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

void UInGameMenuWidget::OnLeaveGameButtonPressed()
{
	MenuSwitcher->SetActiveWidget(ConfirmLeaveGameMenu);
}

void UInGameMenuWidget::OnLeaveGameAcceptButtonPressed()
{
	if (MenuInterface)
	{
		MenuInterface->ExitToMainMenu();
	}
}

void UInGameMenuWidget::OnLeaveGameCancelButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

void UInGameMenuWidget::OnExitButtonPressed()
{
	MenuSwitcher->SetActiveWidget(ConfirmExitMenu);
}

void UInGameMenuWidget::OnExitAcceptButtonPressed()
{
	if (MenuInterface)
	{
		MenuInterface->ExitToDesktop();
	}
}

void UInGameMenuWidget::OnExitCancelButtonPressed()
{
	MenuSwitcher->SetActiveWidget(InGameMenu);
}

FReply UInGameMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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
