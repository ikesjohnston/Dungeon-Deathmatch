// Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsMenuWidget.h"
#include "DungeonGameInstance.h"
#include "InputBindingEditorRow.h"
#include "SettingsGlobals.h"

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

USettingsMenuWidget::USettingsMenuWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool USettingsMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(TabSwitcher != nullptr))
	{
		return false;
	}

	if (!ensure(GraphicsTabButton != nullptr))
	{
		return false;
	}
	GraphicsTabButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnGraphicsTabButtonPressed);

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

	ResolutionDropdown->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnResolutionSelectionChanged);

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

	DisplayModeDropdown->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnDisplayModeSelectionChanged);

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
	FrameLockDropdown->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnFrameLockSelectionChanged);

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
	VSyncToggle->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnVSyncToggleStateChanged);

	if (!ensure(SoundTabButton != nullptr))
	{
		return false;
	}
	SoundTabButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSoundTabButtonPressed);

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		return false;

	}
	FDungeonAudioSettings AudioSettings = GameInstance->GetAudioSettings();

	if (!ensure(MasterSoundToggle != nullptr))
	{
		return false;
	}
	MasterSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.MasterVolumeEnabled));
	MasterSoundToggle->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnMasterSoundToggleStateChanged);

	if (!ensure(MasterSoundSlider != nullptr))
	{
		return false;
	}
	MasterSoundSlider->SetValue(AudioSettings.MasterVolume);
	MasterSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnMasterSoundSliderValueChanged);

	if (!ensure(MasterSoundLevel != nullptr))
	{
		return false;
	}
	MasterSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.MasterVolume * SLIDER_VALUE_MULTIPLIER))));

	if (!ensure(EffectsSoundToggle != nullptr))
	{
		return false;
	}
	EffectsSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.EffectsVolumeEnabled));
	EffectsSoundToggle->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnEffectsSoundToggleStateChanged);

	if(!ensure(EffectsSoundSlider != nullptr))
	{
		return false;
	}
	EffectsSoundSlider->SetValue(AudioSettings.EffectsVolume);
	EffectsSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnEffectsSoundSliderValueChanged);

	if (!ensure(EffectsSoundLevel != nullptr))
	{
		return false;
	}
	EffectsSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.EffectsVolume * SLIDER_VALUE_MULTIPLIER))));

	if (!ensure(UISoundToggle != nullptr))
	{
		return false;
	}
	UISoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.UIVolumeEnabled));
	UISoundToggle->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnUISoundToggleStateChanged);

	if(!ensure(UISoundSlider != nullptr))
	{
		return false;
	}
	UISoundSlider->SetValue(AudioSettings.UIVolume);
	UISoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnUISoundSliderValueChanged);

	if (!ensure(UISoundLevel != nullptr))
	{
		return false;
	}
	UISoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.UIVolume * SLIDER_VALUE_MULTIPLIER))));

	if (!ensure(MusicSoundToggle != nullptr))
	{
		return false;
	}
	MusicSoundToggle->SetCheckedState(ECheckBoxState(AudioSettings.MusicVolumeEnabled));
	MusicSoundToggle->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnMusicSoundToggleStateChanged);

	if (!ensure(MusicSoundSlider != nullptr))
	{
		return false;
	}
	MusicSoundSlider->SetValue(AudioSettings.MusicVolume);
	MusicSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnMusicSoundSliderValueChanged);

	if (!ensure(MusicSoundLevel != nullptr))
	{
		return false;
	}
	MusicSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(AudioSettings.MusicVolume * SLIDER_VALUE_MULTIPLIER))));

	if (!ensure(SoundTab != nullptr))
	{
		return false;
	}

	if (!ensure(ControlsTabButton != nullptr))
	{
		return false;
	}
	ControlsTabButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnControlsTabButtonPressed);

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
		TMap<FName, FDungeonKeyBind> ProcessedBindings;
		for (FInputActionKeyMapping Action : Actions)
		{
			// Ignore gamepad bindings and the Escape key as it is reserved
			if (Action.Key.IsGamepadKey() || Action.Key == EKeys::Escape)
			{
				continue;
			}

			FDungeonKeyBind* KeyBindPtr = ProcessedBindings.Find(Action.ActionName);
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
				FDungeonKeyBind KeyBind;
				KeyBind.ActionName = Action.ActionName.ToString();
				KeyBind.PrimaryBinding = Action;
				ProcessedBindings.Add(TTuple<FName, FDungeonKeyBind>(Action.ActionName, KeyBind));
			}
		}

		//Process categorized bindings in the order they are entered in the editor
		for (FName GeneralInput : GeneralInputNames)
		{
			FDungeonKeyBind* GeneralKeyBindPtr = ProcessedBindings.Find(GeneralInput);
			if (GeneralKeyBindPtr)
			{
				UInputBindingEditorRow* InputBindingRow = CreateWidget<UInputBindingEditorRow>(GetWorld(), InputBindingRowClass);
					
				InputBindingRow->Setup(*GeneralKeyBindPtr);
				GeneralInputBindingList->AddChild(InputBindingRow);
			}
		}

		for (FName CombatInput : CombatInputNames)
		{
			FDungeonKeyBind* CombatKeyBindPtr = ProcessedBindings.Find(CombatInput);
			if (CombatKeyBindPtr)
			{
				UInputBindingEditorRow* InputBindingRow = CreateWidget<UInputBindingEditorRow>(GetWorld(), InputBindingRowClass);

				InputBindingRow->Setup(*CombatKeyBindPtr);
				CombatInputBindingList->AddChild(InputBindingRow);
			}
		}

		for (FName UIInput : UserInterfaceInputNames)
		{
			FDungeonKeyBind* UIKeyBindPtr = ProcessedBindings.Find(UIInput);
			if (UIKeyBindPtr)
			{
				UInputBindingEditorRow* InputBindingRow = CreateWidget<UInputBindingEditorRow>(GetWorld(), InputBindingRowClass);

				InputBindingRow->Setup(*UIKeyBindPtr);
				UIInputBindingList->AddChild(InputBindingRow);
			}
		}
	}

	if (!ensure(GameplayTabButton != nullptr))
	{
		return false;
	}
	GameplayTabButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnGameplayTabButtonPressed);

	if (!ensure(GameplayTab != nullptr))
	{
		return false;
	}

	if (!ensure(ApplyButton != nullptr))
	{
		return false;
	}
	ApplyButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnApplyButtonPressed);
	ApplyButton->SetIsEnabled(false);

	if (!ensure(BackButton != nullptr))
	{
		return false;
	}
	BackButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnBackButtonPressed);

	return Result;
}

void USettingsMenuWidget::OnGraphicsTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(GraphicsTab);
}

void USettingsMenuWidget::OnResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnDisplayModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnFrameLockSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnVSyncToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnSoundTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(SoundTab);
}

void USettingsMenuWidget::OnMasterSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnMasterSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	MasterSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void USettingsMenuWidget::OnEffectsSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnEffectsSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	EffectsSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void USettingsMenuWidget::OnUISoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnUISoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	UISoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void USettingsMenuWidget::OnMusicSoundToggleStateChanged(bool bIsChecked)
{
	ApplyButton->SetIsEnabled(true);
}

void USettingsMenuWidget::OnMusicSoundSliderValueChanged(float Value)
{
	ApplyButton->SetIsEnabled(true);
	MusicSoundLevel->SetText(FText::FromString(FString::FromInt((int32)(Value * SLIDER_VALUE_MULTIPLIER))));
}

void USettingsMenuWidget::OnControlsTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(ControlsTab);
}

void USettingsMenuWidget::OnGameplayTabButtonPressed()
{
	TabSwitcher->SetActiveWidget(GameplayTab);
}

void USettingsMenuWidget::OnApplyButtonPressed()
{
	ApplyButton->SetIsEnabled(false);

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

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		FDungeonAudioSettings AudioSettings;

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

		GameInstance->SetAudioSettings(AudioSettings);
	}
	
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

void USettingsMenuWidget::OnBackButtonPressed()
{
	OnBackButtonClicked.Broadcast();
}
