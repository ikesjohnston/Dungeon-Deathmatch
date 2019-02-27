// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonMenuWidget.h"
#include "InGameMenuWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class IMenuInterface;

/**
 * Widget class for the in game menu and all of its sub menus
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInGameMenuWidget : public UDungeonMenuWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* InGameMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsMenuBackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* ConfirmLeaveGameMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameAcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameCancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* ConfirmExitMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitAcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitCancelButton;

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnResumeButtonPressed();

	UFUNCTION()
	void OnSettingsButtonPressed();

	UFUNCTION()
	void OnSettingsMenuBackButtonPressed();

	UFUNCTION()
	void OnLeaveGameButtonPressed();

	UFUNCTION()
	void OnLeaveGameAcceptButtonPressed();

	UFUNCTION()
	void OnLeaveGameCancelButtonPressed();

	UFUNCTION()
	void OnExitButtonPressed();

	UFUNCTION()
	void OnExitAcceptButtonPressed();

	UFUNCTION()
	void OnExitCancelButtonPressed();
};
