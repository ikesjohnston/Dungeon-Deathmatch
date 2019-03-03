// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DungeonMenuWidget.h"
#include "NetworkGlobals.h"
#include "MainMenuWidget.generated.h"

class UButton;
class IMenuInterface;
class UWidgetSwitcher;
class UEditableTextBox;
class UPanelWidget;
class UServerBrowserRowWidget;
class UComboBoxString;
class UComboBox;
class USettingsMenuWidget;

/**
 * Widget class for the main menu and all sub menus
 */
UCLASS()
class DUNGEONDEATHMATCH_API UMainMenuWidget : public UDungeonMenuWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuHostButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* HostMenuGameNameField;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* GameTypeDropdown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* GameSizeDropdown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* MapDropdown;

	UPROPERTY(meta = (BindWidget))
	UButton* HostMenuHostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HostMenuBackButton;

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
	UWidget* ServerListDisplay;

	UPROPERTY(meta = (BindWidget))
	UWidget* ServerListRefreshDisplay;

	UPROPERTY(meta = (BindWidget))
	UWidget* ServerListEmptyDisplay;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	USettingsMenuWidget* SettingsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* ConfirmExitMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitAcceptButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitCancelButton;

private:
	/** Widget class to display properties for a single server in the server browser */
	TSubclassOf<UUserWidget> ServerDetailsWidgetClass;

	UPROPERTY()
	UServerBrowserRowWidget* ServerDetailsWidget;

	/** The index of the currently selected server, if any, from the server browser */
	TOptional<uint32> SelectedServerIndex;

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	/** Creates row widgets for each found server and adds it to the server list */
	void PopulateServerList(TArray<FServerData> SearchResults);

	void SelectServerIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnMainMenuHostButtonPressed();

	UFUNCTION()
	void OnHostMenuHostButtonPressed();

	UFUNCTION()
	void OnHostMenuBackButtonPressed();

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

	/** Sets properties used to determine cosmetic changes to server rows in blueprint */
	UFUNCTION()
	void UpdateServerRowWidgets();

	UFUNCTION()
	void OnHostMenuGameNameFieldChanged(const FText& Text);

	UFUNCTION()
	void OnGameTypeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnGameSizeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnMapSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
