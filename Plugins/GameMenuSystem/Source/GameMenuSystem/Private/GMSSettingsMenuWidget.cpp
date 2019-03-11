// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSSettingsMenuWidget.h"
#include "GMSMenuInterface.h"
#include "GMSInputBindingEditorRow.h"
#include "GMSSettingsGlobals.h"

#include <ComboBoxString.h>
#include <Button.h>
#include <WidgetSwitcher.h>
#include <CheckBox.h>
#include <Slider.h>
#include <TextBlock.h>
#include <Kismet/GameplayStatics.h>
#include <UnrealEngine.h>
#include <Engine/Engine.h>
#include <GameFramework/InputSettings.h>
#include <GameFramework/PlayerInput.h>
#include <LogMacros.h>
#include <GameFramework/GameUserSettings.h>

const static FString	FORMAT_RESOLUTION							= TEXT("{0}x{1}");

const static FString	DISPLAY_MODE_OPTION_FULLSCREEN				= TEXT("Fullscreen");
const static FString	DISPLAY_MODE_OPTION_BORDERLESSWINDOWED		= TEXT("Borderless Windowed");
const static FString	DISPLAY_MODE_OPTION_WINDOWED				= TEXT("Windowed");

const static FString	FRAME_LOCK_UNLOCKED							= TEXT("Unlimited");
const static FString	FRAME_LOCK_30								= TEXT("30");
const static FString	FRAME_LOCK_60								= TEXT("60");

const static int32		SLIDER_VALUE_MULTIPLIER						= 100;

UGMSSettingsMenuWidget::UGMSSettingsMenuWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bAreSoundWidgetsInitialized = false;
}

bool UGMSSettingsMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!InitializeBase() || !InitializeGraphicsTab() || !InitializeSoundTab() || !InitializeControlsTab() || !InitializeGameplayTab())
	{
		Result = false;
	}

	return Result;
}

bool UGMSSettingsMenuWidget::InitializeBase()
{
	if (!ensure(TabSwitcher != nullptr))
	{
		return false;
	}

	if (!ensure(ApplyButton != nullptr))
	{
		return false;
	}
	ApplyButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnApplyButtonPressed);
	ApplyButton->SetIsEnabled(false);

	if (!ensure(BackButton != nullptr))
	{
		return false;
	}
	BackButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnBackButtonPressed);

	return true;
}

bool UGMSSettingsMenuWidget::InitializeGraphicsTab()
{
	if (!ensure(GraphicsTabButton != nullptr))
	{
		return false;
	}
	GraphicsTabButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnGraphicsTabButtonPressed);

	if (!ensure(GraphicsTab != nullptr))
	{
		return false;
	}

	if (!ensure(ResolutionDropdown != nullptr))
	{
		return false;
	}

	UGameUserSettings* UserSettings = nullptr;
	if (GEngine)
	{
		UserSettings = GEngine->GetGameUserSettings();
	}

	// Add all resolution options for current display adapter, ignoring different refresh rates
	FScreenResolutionArray AvailableResolutions;
	FString ResOption;
	RHIGetAvailableResolutions(AvailableResolutions, true);
	for (const FScreenResolutionRHI& Resolution : AvailableResolutions)
	{
		TArray<FStringFormatArg> FormatArgs;
		FormatArgs.Add(FStringFormatArg(Resolution.Width));
		FormatArgs.Add(FStringFormatArg(Resolution.Height));

		ResOption = FString::Format(*FORMAT_RESOLUTION, FormatArgs);
		ResolutionDropdown->AddOption(ResOption);
	}

	// Set the selected option to the current resolution
	TArray<FStringFormatArg> FormatArgs;
	FormatArgs.Add(FStringFormatArg(GSystemResolution.ResX));
	FormatArgs.Add(FStringFormatArg(GSystemResolution.ResY));
	ResOption = FString::Format(*FORMAT_RESOLUTION, FormatArgs);
	ResolutionDropdown->SetSelectedOption(ResOption);

	ResolutionDropdown->OnSelectionChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnResolutionSelectionChanged);

	if (!ensure(DisplayModeDropdown != nullptr))
	{
		return false;
	}
	DisplayModeDropdown->AddOption(DISPLAY_MODE_OPTION_FULLSCREEN);
	DisplayModeDropdown->AddOption(DISPLAY_MODE_OPTION_BORDERLESSWINDOWED);
	DisplayModeDropdown->AddOption(DISPLAY_MODE_OPTION_WINDOWED);

	if (UserSettings)
	{
		EWindowMode::Type WindowMode = UserSettings->GetFullscreenMode();
		switch (WindowMode)
		{
		case EWindowMode::Fullscreen:
			DisplayModeDropdown->SetSelectedOption(DISPLAY_MODE_OPTION_FULLSCREEN);
			break;
		case EWindowMode::WindowedFullscreen:
			DisplayModeDropdown->SetSelectedOption(DISPLAY_MODE_OPTION_BORDERLESSWINDOWED);
			break;
		case EWindowMode::Windowed:
			DisplayModeDropdown->SetSelectedOption(DISPLAY_MODE_OPTION_WINDOWED);
			break;
		default:
			break;
		}
	}
	else
	{
		DisplayModeDropdown->SetSelectedOption(DISPLAY_MODE_OPTION_FULLSCREEN);
	}

	DisplayModeDropdown->OnSelectionChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnDisplayModeSelectionChanged);

	if (!ensure(FrameLockDropdown != nullptr))
	{
		return false;
	}
	FrameLockDropdown->AddOption(FRAME_LOCK_UNLOCKED);
	FrameLockDropdown->AddOption(FRAME_LOCK_30);
	FrameLockDropdown->AddOption(FRAME_LOCK_60);

	if (UserSettings)
	{
		int32 FrameRateLimit = UserSettings->GetFrameRateLimit();
		FString SelectedFrameLimit;
		if (FrameRateLimit > 0)
		{
			SelectedFrameLimit = FString::FromInt(FrameRateLimit);
		}
		else
		{
			SelectedFrameLimit = FRAME_LOCK_UNLOCKED;
		}
		FrameLockDropdown->SetSelectedOption(SelectedFrameLimit);
	}
	else
	{
		FrameLockDropdown->SetSelectedOption(FRAME_LOCK_UNLOCKED);
	}
	FrameLockDropdown->OnSelectionChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnFrameLockSelectionChanged);

	if (!ensure(VSyncToggle != nullptr))
	{
		return false;
	}
	if (UserSettings)
	{
		VSyncToggle->SetCheckedState(ECheckBoxState(UserSettings->IsVSyncEnabled()));
	}
	else
	{
		VSyncToggle->SetCheckedState(ECheckBoxState::Checked);
	}
	VSyncToggle->OnCheckStateChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnVSyncToggleStateChanged);

	return true;
}

bool UGMSSettingsMenuWidget::InitializeSoundTab()
{
	if (!ensure(SoundTabButton != nullptr))
	{
		return false;
	}
	SoundTabButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnSoundTabButtonPressed);

	if (!ensure(SoundTab != nullptr))
	{
		return false;
	}

	InitializeSoundWidgets();

	return true;
}

void UGMSSettingsMenuWidget::InitializeSoundWidgets()
{
	if (!bAreSoundWidgetsInitialized)
	{

		if (MenuInterface)
		{
			FGMSAudioSettings AudioSettings = MenuInterface->GetAudioSettings();

			if (!ensure(MasterSoundToggle != nullptr))
			{
				return;
			}
			MasterSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.MasterVolumeEnabled));
			MasterSoundToggle->OnCheckStateChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnMasterSoundToggleStateChanged);

			if (!ensure(MasterSoundSlider != nullptr))
			{
				return;
			}
			MasterSoundSlider->SetValue(AudioSettings.MasterVolume);
			MasterSoundSlider->OnValueChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnMasterSoundSliderValueChanged);

			if (!ensure(MasterSoundLevel != nullptr))
			{
				return;
			}
			MasterSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.MasterVolume * SLIDER_VALUE_MULTIPLIER))));

			if (!ensure(EffectsSoundToggle != nullptr))
			{
				return;
			}
			EffectsSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.EffectsVolumeEnabled));
			EffectsSoundToggle->OnCheckStateChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnEffectsSoundToggleStateChanged);

			if (!ensure(EffectsSoundSlider != nullptr))
			{
				return;
			}
			EffectsSoundSlider->SetValue(AudioSettings.EffectsVolume);
			EffectsSoundSlider->OnValueChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnEffectsSoundSliderValueChanged);

			if (!ensure(EffectsSoundLevel != nullptr))
			{
				return;
			}
			EffectsSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.EffectsVolume * SLIDER_VALUE_MULTIPLIER))));

			if (!ensure(UISoundToggle != nullptr))
			{
				return;
			}
			UISoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.UIVolumeEnabled));
			UISoundToggle->OnCheckStateChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnUISoundToggleStateChanged);

			if (!ensure(UISoundSlider != nullptr))
			{
				return;
			}
			UISoundSlider->SetValue(AudioSettings.UIVolume);
			UISoundSlider->OnValueChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnUISoundSliderValueChanged);

			if (!ensure(UISoundLevel != nullptr))
			{
				return;
			}
			UISoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.UIVolume * SLIDER_VALUE_MULTIPLIER))));

			if (!ensure(MusicSoundToggle != nullptr))
			{
				return;
			}
			MusicSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.MusicVolumeEnabled));
			MusicSoundToggle->OnCheckStateChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnMusicSoundToggleStateChanged);

			if (!ensure(MusicSoundSlider != nullptr))
			{
				return;
			}
			MusicSoundSlider->SetValue(AudioSettings.MusicVolume);
			MusicSoundSlider->OnValueChanged.AddDynamic(this, &UGMSSettingsMenuWidget::OnMusicSoundSliderValueChanged);

			if (!ensure(MusicSoundLevel != nullptr))
			{
				return;
			}
			MusicSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.MusicVolume * SLIDER_VALUE_MULTIPLIER))));

			bAreSoundWidgetsInitialized = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGMSSettingsMenuWidget::InitializeSoundTab - MenuInterface is null."))
		}
	}
}

bool UGMSSettingsMenuWidget::InitializeControlsTab()
{
	if (!ensure(ControlsTabButton != nullptr))
	{
		return false;
	}
	ControlsTabButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnControlsTabButtonPressed);

	if (!ensure(ControlsTab != nullptr))
	{
		return false;
	}

	if (!ensure(GeneralInputBindingList != nullptr))
	{
		return false;
	}

	if (!ensure(CombatInputBindingList != nullptr))
	{
		return false;
	}

	if (!ensure(UIInputBindingList != nullptr))
	{
		return false;
	}

	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	if (InputBindingRowClass)
	{
		TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
		TMap<FName, FGMSKeyBind> ProcessedBindings;
		for (FInputActionKeyMapping Action : Actions)
		{
			// Ignore gamepad bindings and the Escape key as it is reserved
			if (Action.Key.IsGamepadKey() || Action.Key == EKeys::Escape)
			{
				continue;
			}

			FGMSKeyBind* KeyBindPtr = ProcessedBindings.Find(Action.ActionName);
			if (KeyBindPtr)
			{
				// Only add first two bindings to action
				if (KeyBindPtr->SecondaryBinding.ActionName.IsNone())
				{
					KeyBindPtr->SecondaryBinding = Action;
				}
			}
			else
			{
				FGMSKeyBind KeyBind;
				KeyBind.ActionName = Action.ActionName.ToString();
				KeyBind.PrimaryBinding = Action;
				ProcessedBindings.Add(TTuple<FName, FGMSKeyBind>(Action.ActionName, KeyBind));
			}
		}

		//Process categorized bindings in the order they are entered in the editor
		for (FName GeneralInput : GeneralInputNames)
		{
			FGMSKeyBind* GeneralKeyBindPtr = ProcessedBindings.Find(GeneralInput);
			if (GeneralKeyBindPtr)
			{
				UGMSInputBindingEditorRow* InputBindingRow = CreateWidget<UGMSInputBindingEditorRow>(GetWorld(), InputBindingRowClass);

				InputBindingRow->Setup(*GeneralKeyBindPtr);
				GeneralInputBindingList->AddChild(InputBindingRow);
			}
		}

		for (FName CombatInput : CombatInputNames)
		{
			FGMSKeyBind* CombatKeyBindPtr = ProcessedBindings.Find(CombatInput);
			if (CombatKeyBindPtr)
			{
				UGMSInputBindingEditorRow* InputBindingRow = CreateWidget<UGMSInputBindingEditorRow>(GetWorld(), InputBindingRowClass);

				InputBindingRow->Setup(*CombatKeyBindPtr);
				CombatInputBindingList->AddChild(InputBindingRow);
			}
		}

		for (FName UIInput : UserInterfaceInputNames)
		{
			FGMSKeyBind* UIKeyBindPtr = ProcessedBindings.Find(UIInput);
			if (UIKeyBindPtr)
			{
				UGMSInputBindingEditorRow* InputBindingRow = CreateWidget<UGMSInputBindingEditorRow>(GetWorld(), InputBindingRowClass);

				InputBindingRow->Setup(*UIKeyBindPtr);
				UIInputBindingList->AddChild(InputBindingRow);
			}
		}
	}

	return true;
}

bool UGMSSettingsMenuWidget::InitializeGameplayTab()
{
	if (!ensure(GameplayTabButton != nullptr))
	{
		return false;
	}
	GameplayTabButton->OnClicked.AddDynamic(this, &UGMSSettingsMenuWidget::OnGameplayTabButtonPressed);

	if (!ensure(GameplayTab != nullptr))
	{
		return false;
	}

	return true;
}

void UGMSSettingsMenuWidget::OnGraphicsTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(GraphicsTab);
}

void UGMSSettingsMenuWidget::OnResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnDisplayModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnFrameLockSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnVSyncToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnSoundTabButtonPressed()
{
	InitializeSoundWidgets();
	TabSwitcher->SetActiveWidget(SoundTab);
}

void UGMSSettingsMenuWidget::OnMasterSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnMasterSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	MasterSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void UGMSSettingsMenuWidget::OnEffectsSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnEffectsSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	EffectsSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void UGMSSettingsMenuWidget::OnUISoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnUISoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	UISoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void UGMSSettingsMenuWidget::OnMusicSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void UGMSSettingsMenuWidget::OnMusicSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	MusicSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void UGMSSettingsMenuWidget::OnControlsTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(ControlsTab);
}

void UGMSSettingsMenuWidget::OnGameplayTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(GameplayTab);
}

void UGMSSettingsMenuWidget::OnApplyButtonPressed()
{
	ApplyButton->SetIsEnabled(false);

	ApplyGraphicsSettings();
	ApplyAudioSettings();
}

void UGMSSettingsMenuWidget::ApplyGraphicsSettings()
{
	FString Resolution = ResolutionDropdown->GetSelectedOption();
	int32 SeparatorIndex;
	Resolution.FindChar('x', SeparatorIndex);
	int32 ScreenWidth = FCString::Atoi(*Resolution.Left(SeparatorIndex));
	int32 ScreenHeight = FCString::Atoi(*Resolution.Right(Resolution.Len() - SeparatorIndex - 1));
	FString SelectedDisplayMode = DisplayModeDropdown->GetSelectedOption();
	int32 DisplayModeIndex = DisplayModeDropdown->FindOptionIndex(SelectedDisplayMode);

	int32 FrameLock = 0;
	FString SelectedFrameLock = FrameLockDropdown->GetSelectedOption();
	if (!SelectedFrameLock.Equals(FRAME_LOCK_UNLOCKED))
	{
		FrameLock = FCString::Atoi(*SelectedFrameLock);
	}

	bool VSyncEnabled = VSyncToggle->IsChecked();

	if (GEngine)
	{
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		UserSettings->SetScreenResolution(FIntPoint(ScreenWidth, ScreenHeight));
		UserSettings->SetFullscreenMode(EWindowMode::Type(DisplayModeIndex));
		UserSettings->SetFrameRateLimit(FrameLock);
		UserSettings->SetVSyncEnabled(VSyncEnabled);
		UserSettings->ConfirmVideoMode();
		UserSettings->ApplySettings(false);
	}
}

void UGMSSettingsMenuWidget::ApplyAudioSettings()
{
	FGMSAudioSettings AudioSettings;

	float MasterVolume = MasterSoundSlider->GetValue();
	bool MasterSoundEnabled = false;
	if (MasterSoundToggle->GetCheckedState() == ECheckBoxState::Checked)
	{
		MasterSoundEnabled = true;
	}
	AudioSettings.MasterVolumeEnabled = MasterSoundEnabled;
	AudioSettings.MasterVolume = MasterVolume;

	float EffectsVolume = EffectsSoundSlider->GetValue();
	bool EffectsSoundEnabled = false;
	if (EffectsSoundToggle->GetCheckedState() == ECheckBoxState::Checked)
	{
		EffectsSoundEnabled = true;
	}
	AudioSettings.EffectsVolumeEnabled = EffectsSoundEnabled;
	AudioSettings.EffectsVolume = EffectsVolume;

	float UIVolume = UISoundSlider->GetValue();
	bool UISoundEnabled = false;
	if (UISoundToggle->GetCheckedState() == ECheckBoxState::Checked)
	{
		UISoundEnabled = true;
	}
	AudioSettings.UIVolumeEnabled = UISoundEnabled;
	AudioSettings.UIVolume = UIVolume;

	float MusicVolume = MusicSoundSlider->GetValue();;
	bool MusicSoundEnabled = false;
	if (MusicSoundToggle->GetCheckedState() == ECheckBoxState::Checked)
	{
		MusicSoundEnabled = true;
	}
	AudioSettings.MusicVolumeEnabled = MusicSoundEnabled;
	AudioSettings.MusicVolume = MusicVolume;

	MenuInterface->SetAudioSettings(AudioSettings);
}

void UGMSSettingsMenuWidget::OnBackButtonPressed()
{
	OnBackButtonClicked.Broadcast();
}
