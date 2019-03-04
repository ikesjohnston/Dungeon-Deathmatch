// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonMenuWidget.h"
#include "InGameMenuWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class IMenuInterface;
class USettingsMenuWidget;

/**
 * Widget class for the in game menu and all of its sub menus
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInGameMenuWidget : public UDungeonMenuWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* ConfirmLeaveGameMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameAcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGameCancelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
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

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
