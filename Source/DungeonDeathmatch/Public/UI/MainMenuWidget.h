// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonMenuWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class IMenuInterface;
class UWidgetSwitcher;
class UEditableTextBox;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UMainMenuWidget : public UDungeonMenuWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuHostButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuJoinButton;
	
	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* IPAddressField;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuJoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuBackButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsMenuBackButton;

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
	void OnMainMenuHostButtonPressed();

	UFUNCTION()
	void OnMainMenuJoinButtonPressed();

	UFUNCTION()
	void OnJoinMenuJoinButtonPressed();

	UFUNCTION()
	void OnJoinMenuBackButtonPressed();

	UFUNCTION()
	void OnSettingsButtonPressed();

	UFUNCTION()
	void OnSettingsMenuBackButtonPressed();

	UFUNCTION()
	void OnExitButtonPressed();

	UFUNCTION()
	void OnExitAcceptButtonPressed();

	UFUNCTION()
	void OnExitCancelButtonPressed();
};
