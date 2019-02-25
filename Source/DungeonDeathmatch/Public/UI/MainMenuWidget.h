// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonMenuWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class IMenuInterface;
class UWidgetSwitcher;
class UEditableTextBox;
class UPanelWidget;
class UServerBrowserLineWidget;

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
	UPanelWidget* ServerList;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuJoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuRefreshButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinMenuBackButton;
	
	UPROPERTY(meta = (BindWidget))
	UWidget* ServerListRefreshDisplay;

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

private:
	TSubclassOf<UUserWidget> ServerDetailsWidgetClass;

	UPROPERTY()
	UServerBrowserLineWidget* ServerDetailsWidget;

	TOptional<uint32> SelectedServerIndex;

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	void PopulateServerList(TArray<FOnlineSessionSearchResult> SearchResults);

	void SelectServerIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnMainMenuHostButtonPressed();

	UFUNCTION()
	void OnMainMenuJoinButtonPressed();

	UFUNCTION()
	void OnJoinMenuJoinButtonPressed();

	UFUNCTION()
	void OnJoinMenuRefreshButtonPressed();

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

	UFUNCTION()
	void RefreshServerList();
};
