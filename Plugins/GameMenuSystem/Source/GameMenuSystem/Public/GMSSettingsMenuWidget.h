// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GMSMenuWidgetBase.h"
#include "GMSSettingsMenuWidget.generated.h"

class UComboBoxString;
class UButton;
class UWidgetSwitcher;
class USoundClass;
class USlider;
class UCheckBox;
class UTextBlock;
class USoundMix;

class UGMSInputBindingEditorRow;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackButtonClickedEvent);

/**
 * Widget class for the settings menu and all sub menus
 */
UCLASS()
class GAMEMENUSYSTEM_API UGMSSettingsMenuWidget : public UGMSMenuWidgetBase
{
	GENERATED_BODY()
	
public:
	FOnBackButtonClickedEvent OnBackButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* TabSwitcher;

	UPROPERTY(meta = (BindWidget))
	UButton* GraphicsTabButton;
	
	UPROPERTY(meta = (BindWidget))
	UWidget* GraphicsTab;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ResolutionDropdown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* DisplayModeDropdown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* FrameLockDropdown;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* VSyncToggle;

	UPROPERTY(meta = (BindWidget))
	UButton* SoundTabButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* SoundTab;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MasterSoundToggle;

	UPROPERTY(meta = (BindWidget))
	USlider* MasterSoundSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MasterSoundLevel;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* EffectsSoundToggle;

	UPROPERTY(meta = (BindWidget))
	USlider* EffectsSoundSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EffectsSoundLevel;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* UISoundToggle;

	UPROPERTY(meta = (BindWidget))
	USlider* UISoundSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UISoundLevel;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MusicSoundToggle;

	UPROPERTY(meta = (BindWidget))
	USlider* MusicSoundSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MusicSoundLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* ControlsTabButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* ControlsTab;

	UPROPERTY(EditAnywhere)
	TArray<FName> GeneralInputNames;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* GeneralInputBindingList;

	UPROPERTY(EditAnywhere)
	TArray<FName>CombatInputNames;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* CombatInputBindingList;

	UPROPERTY(EditAnywhere)
	TArray<FName> UserInterfaceInputNames;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* UIInputBindingList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGMSInputBindingEditorRow> InputBindingRowClass;

	UPROPERTY(meta = (BindWidget))
	UButton* GameplayTabButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* GameplayTab;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ApplyButton;

private:
	bool bAreSoundWidgetsInitialized;

public:
	UGMSSettingsMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool Initialize() override;

	bool InitializeBase();

	bool InitializeGraphicsTab();

	bool InitializeSoundTab();

	void InitializeSoundWidgets();

	bool InitializeControlsTab();

	bool InitializeGameplayTab();

	UFUNCTION()
	void OnGraphicsTabButtonPressed();

	UFUNCTION()
	void OnResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnDisplayModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnFrameLockSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnVSyncToggleStateChanged(bool bIsChecked);

	UFUNCTION()
	void OnSoundTabButtonPressed();

	UFUNCTION()
	void OnMasterSoundToggleStateChanged(bool bIsChecked);

	UFUNCTION()
	void OnMasterSoundSliderValueChanged(float Value);

	UFUNCTION()
	void OnEffectsSoundToggleStateChanged(bool bIsChecked);

	UFUNCTION()
	void OnEffectsSoundSliderValueChanged(float Value);

	UFUNCTION()
	void OnUISoundToggleStateChanged(bool bIsChecked);

	UFUNCTION()
	void OnUISoundSliderValueChanged(float Value);

	UFUNCTION()
	void OnMusicSoundToggleStateChanged(bool bIsChecked);

	UFUNCTION()
	void OnMusicSoundSliderValueChanged(float Value);

	UFUNCTION()
	void OnControlsTabButtonPressed();

	UFUNCTION()
	void OnGameplayTabButtonPressed();

	UFUNCTION()
	void OnApplyButtonPressed();

	void ApplyGraphicsSettings();

	void ApplyAudioSettings();

	UFUNCTION()
	void OnBackButtonPressed();
};
