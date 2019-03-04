// Fill out your copyright notice in the Description page of Project Settings.

#include "InputBindingEditorRow.h"
#include "SettingsGlobals.h"

#include <Button.h>
#include <TextBlock.h>
#include <GameFramework/PlayerInput.h>
#include <Events.h>
#include <GameFramework/InputSettings.h>


const static FString INPUT_MOD_SHIFT = TEXT("Shift + ");
const static FString INPUT_MOD_CTRL = TEXT("Ctrl + ");
const static FString INPUT_MOD_ALT = TEXT("Alt + ");

bool UInputBindingEditorRow::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(ActionNameText != nullptr))
	{
		return false;
	}

	if (!ensure(PrimaryBindingButton != nullptr))
	{
		return false;
	}
	PrimaryBindingButton->OnClicked.AddDynamic(this, &UInputBindingEditorRow::OnPrimaryBindingButtonPressed);

	if (!ensure(PrimaryBindingText != nullptr))
	{
		return false;
	}

	if (!ensure(SecondaryBindingButton != nullptr))
	{
		return false;
	}
	SecondaryBindingButton->OnClicked.AddDynamic(this, &UInputBindingEditorRow::OnSecondaryBindingButtonPressed);

	if (!ensure(SecondaryBindingText != nullptr))
	{
		return false;
	}

	return Result;
}

void UInputBindingEditorRow::Setup(FDungeonKeyBind Binding)
{
	ActionNameText->SetText(FText::FromString(Binding.ActionName));

	PrimaryBindingText->SetText(FText::FromString(GetBindingString(Binding.PrimaryBinding)));
	SecondaryBindingText->SetText(FText::FromString(GetBindingString(Binding.SecondaryBinding)));
}

void UInputBindingEditorRow::OnPrimaryBindingButtonPressed()
{
	if (!bIsWaitingForPrimaryBinding)
	{
		bIsWaitingForPrimaryBinding = true;
		PrimaryBindingButton->SetIsEnabled(false);
		PrimaryBindingText->SetVisibility(ESlateVisibility::Collapsed);
		APlayerController* OwningController = GetOwningPlayer();
		if (OwningController)
		{
			OwningController->bShowMouseCursor = false;
		}
	}
}

void UInputBindingEditorRow::OnPrimaryBindingStop()
{
	bIsWaitingForPrimaryBinding = false;
	PrimaryBindingButton->SetIsEnabled(true);
	PrimaryBindingText->SetVisibility(ESlateVisibility::Visible);
	APlayerController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		OwningController->bShowMouseCursor = true;
	}
}

void UInputBindingEditorRow::BindPrimaryInput(FKeyEvent KeyEvent)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return;

	FName ActionName = FName(*ActionNameText->GetText().ToString());
	TArray<FInputActionKeyMapping> KeyMappings;
	Settings->GetActionMappingByName(ActionName, KeyMappings);

	for (FInputActionKeyMapping& Mapping : KeyMappings)
	{
		Settings->RemoveActionMapping(Mapping, false);
	}

	FInputActionKeyMapping ReplacementMapping;
	if (KeyMappings.Num() > 0)
	{
		KeyMappings.RemoveAt(0);
	}
	
	ReplacementMapping.ActionName = ActionName;
	ReplacementMapping.bAlt = KeyEvent.IsAltDown();
	ReplacementMapping.bCtrl = KeyEvent.IsControlDown();
	ReplacementMapping.bShift = KeyEvent.IsShiftDown();
	ReplacementMapping.Key = KeyEvent.GetKey();

	KeyMappings.Insert(ReplacementMapping, 0);

	PrimaryBindingText->SetText(FText::FromString(GetBindingString(ReplacementMapping)));

	for (FInputActionKeyMapping& Mapping : KeyMappings)
	{
		Settings->AddActionMapping(Mapping, false);
	}
	
	Settings->SaveKeyMappings();
	Settings->ForceRebuildKeymaps();

	OnPrimaryBindingStop();
}

void UInputBindingEditorRow::OnSecondaryBindingButtonPressed()
{
	if (!bIsWaitingForSecondaryBinding)
	{
		bIsWaitingForSecondaryBinding = true;
		SecondaryBindingButton->SetIsEnabled(false);
		SecondaryBindingText->SetVisibility(ESlateVisibility::Collapsed);
		APlayerController* OwningController = GetOwningPlayer();
		if (OwningController)
		{
			OwningController->bShowMouseCursor = false;
		}
	}
}

void UInputBindingEditorRow::OnSecondaryBindingStop()
{
	bIsWaitingForSecondaryBinding = false;
	SecondaryBindingButton->SetIsEnabled(true);
	SecondaryBindingText->SetVisibility(ESlateVisibility::Visible);
	APlayerController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		OwningController->bShowMouseCursor = true;
	}
}

void UInputBindingEditorRow::BindSecondaryInput(FKeyEvent KeyEvent)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return;

	FName ActionName = FName(*ActionNameText->GetText().ToString());
	TArray<FInputActionKeyMapping> KeyMappings;
	Settings->GetActionMappingByName(ActionName, KeyMappings);

	for (FInputActionKeyMapping& Mapping : KeyMappings)
	{
		Settings->RemoveActionMapping(Mapping, false);
	}

	FInputActionKeyMapping ReplacementMapping;
	if (KeyMappings.Num() > 1)
	{
		KeyMappings.RemoveAt(1);
	}

	ReplacementMapping.ActionName = ActionName;
	ReplacementMapping.bAlt = KeyEvent.IsAltDown();
	ReplacementMapping.bCtrl = KeyEvent.IsControlDown();
	ReplacementMapping.bShift = KeyEvent.IsShiftDown();
	ReplacementMapping.Key = KeyEvent.GetKey();

	KeyMappings.Insert(ReplacementMapping, 1);

	SecondaryBindingText->SetText(FText::FromString(GetBindingString(ReplacementMapping)));

	for (FInputActionKeyMapping& Mapping : KeyMappings)
	{
		Settings->AddActionMapping(Mapping, false);
	}

	Settings->SaveKeyMappings();
	Settings->ForceRebuildKeymaps();

	OnSecondaryBindingStop();
}

FReply UInputBindingEditorRow::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FReply Reply = FReply::Unhandled();

	FKey InKey = InKeyEvent.GetKey();
	if (InKey == EKeys::LeftShift)
	{
		bIsLeftShiftDown = true;
	}
	else if (InKey == EKeys::RightShift)
	{
		bIsRightShiftDown = true;
	}
	else if (InKey == EKeys::LeftControl)
	{
		bIsLeftCtrlDown = true;
	}
	else if (InKey == EKeys::RightControl)
	{
		bIsRightCtrlDown = true;
	}
	else if (InKey == EKeys::LeftAlt)
	{
		bIsLeftAltDown = true;
	}
	else if (InKey == EKeys::RightAlt)
	{
		bIsRightAltDown = true;
	}
	else
	{
		if (bIsWaitingForPrimaryBinding)
		{
			if (InKeyEvent.GetKey() == EKeys::Escape)
			{
				OnPrimaryBindingStop();
			}
			else
			{
				BindPrimaryInput(InKeyEvent);
			}
			Reply = FReply::Handled();
		}
		else if (bIsWaitingForSecondaryBinding)
		{
			if (InKeyEvent.GetKey() == EKeys::Escape)
			{
				OnSecondaryBindingStop();
			}
			else
			{
				BindSecondaryInput(InKeyEvent);
			}
			Reply = FReply::Handled();
		}
		Reply.ReleaseMouseLock();
	}

	return Reply;
}

FReply UInputBindingEditorRow::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	FReply Reply = FReply::Handled();

	FKey InKey = InKeyEvent.GetKey();
	if (InKey == EKeys::LeftShift)
	{
		bIsLeftShiftDown = false;
	}
	else if (InKey == EKeys::RightShift)
	{
		bIsRightShiftDown = false;
	}
	else if (InKey == EKeys::LeftControl)
	{
		bIsLeftCtrlDown = false;
	}
	else if (InKey == EKeys::RightControl)
	{
		bIsRightCtrlDown = false;
	}
	else if (InKey == EKeys::LeftAlt)
	{
		bIsLeftAltDown = false;
	}
	else if (InKey == EKeys::RightAlt)
	{
		bIsRightAltDown = false;
	}
	
	if (!IsModifierDown())
	{
		// Binding to a singular modifier key
		if (bIsWaitingForPrimaryBinding)
		{
			if (InKeyEvent.GetKey() == EKeys::Escape)
			{
				OnPrimaryBindingStop();
			}
			else
			{
				BindPrimaryInput(InKeyEvent);
			}
		}
		else if (bIsWaitingForSecondaryBinding)
		{
			if (InKeyEvent.GetKey() == EKeys::Escape)
			{
				OnSecondaryBindingStop();
			}
			else
			{
				BindSecondaryInput(InKeyEvent);
			}
		}
		Reply.ReleaseMouseLock();
	}

	return Reply;
}

FReply UInputBindingEditorRow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// Construct key event from mouse event; we only need to know the button pressed and any modifiers
	FKey PressedKey = InMouseEvent.GetEffectingButton();
	const FModifierKeysState KeyModifiers = FModifierKeysState(InMouseEvent.IsLeftShiftDown(), InMouseEvent.IsRightShiftDown(), InMouseEvent.IsLeftControlDown(), InMouseEvent.IsRightControlDown(),
		InMouseEvent.IsLeftAltDown(), InMouseEvent.IsRightAltDown(), InMouseEvent.IsLeftCommandDown(), InMouseEvent.IsRightCommandDown(), InMouseEvent.AreCapsLocked());

	FKeyEvent KeyEvent = FKeyEvent(PressedKey, KeyModifiers, 0, false, 0, 0);

	if (bIsWaitingForPrimaryBinding)
	{
		BindPrimaryInput(KeyEvent);
	}
	else if (bIsWaitingForSecondaryBinding)
	{
		BindSecondaryInput(KeyEvent);
	}

	FReply Reply = FReply::Handled();
	Reply.ReleaseMouseLock();

	return Reply;
}

FReply UInputBindingEditorRow::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();

	if (bIsWaitingForPrimaryBinding)
	{
		Reply.LockMouseToWidget(PrimaryBindingButton->TakeWidget());
	}
	else if (bIsWaitingForSecondaryBinding)
	{
		Reply.LockMouseToWidget(SecondaryBindingButton->TakeWidget());
	}

	return Reply;
}

void UInputBindingEditorRow::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	if (bIsWaitingForPrimaryBinding)
	{
		OnPrimaryBindingStop();
	}
	else if (bIsWaitingForSecondaryBinding)
	{
		OnSecondaryBindingStop();
	}
}

FString UInputBindingEditorRow::GetBindingString(FInputActionKeyMapping Binding)
{
	FString InputKey;
	if (Binding.bShift && Binding.Key != EKeys::LeftShift && Binding.Key != EKeys::RightShift)
	{
		InputKey.Append(INPUT_MOD_SHIFT);
	}
	if (Binding.bCtrl && Binding.Key != EKeys::LeftControl && Binding.Key != EKeys::RightControl)
	{
		InputKey.Append(INPUT_MOD_CTRL);
	}
	if (Binding.bAlt && Binding.Key != EKeys::LeftAlt && Binding.Key != EKeys::RightAlt)
	{
		InputKey.Append(INPUT_MOD_ALT);
	}
	InputKey.Append(Binding.Key.GetDisplayName(false).ToString());

	return InputKey;
}

bool UInputBindingEditorRow::IsModifierDown()
{
	return (bIsLeftShiftDown || bIsRightShiftDown || bIsLeftCtrlDown || bIsRightCtrlDown || bIsLeftAltDown || bIsRightAltDown);
}
