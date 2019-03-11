// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GMSSettingsGlobals.h"
#include "GMSInputBindingEditorRow.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class GAMEMENUSYSTEM_API UGMSInputBindingEditorRow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionNameText;

	UPROPERTY(meta = (BindWidget))
	UButton* PrimaryBindingButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PrimaryBindingText;

	UPROPERTY(meta = (BindWidget))
	UButton* SecondaryBindingButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondaryBindingText;

private:
	bool bIsWaitingForPrimaryBinding;
	bool bIsWaitingForSecondaryBinding;
	bool bIsLeftShiftDown;
	bool bIsRightShiftDown;
	bool bIsLeftCtrlDown;
	bool bIsRightCtrlDown;
	bool bIsLeftAltDown;
	bool bIsRightAltDown;

public:
	void Setup(FGMSKeyBind Binding);

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnPrimaryBindingButtonPressed();

	void OnPrimaryBindingStop();

	void BindPrimaryInput(FKeyEvent KeyEvent);

	UFUNCTION()
	void OnSecondaryBindingButtonPressed();

	void OnSecondaryBindingStop();

	void BindSecondaryInput(FKeyEvent KeyEvent);

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

private:
	FString GetBindingString(FInputActionKeyMapping Binding);

	bool IsModifierDown();
};
