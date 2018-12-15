// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameOverlayWidget.h"
#include "UI/CharacterMenuWidget.h"
#include "Blueprint/WidgetTree.h"

UCharacterMenuWidget* UInGameOverlayWidget::GetCharacterMenu()
{
	UCharacterMenuWidget* CharacterMenu = Cast<UCharacterMenuWidget>(WidgetTree->FindWidget(CharacterMenuWidgetName));
	if (!CharacterMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::GetCharacterMenu - No CharacterMenu widget found in %s. Verify that CharacterMenuWidgetName is correctly set."), *GetName());
	}
	return CharacterMenu;
}

void UInGameOverlayWidget::ShowCharacterMenu()
{
	UCharacterMenuWidget* CharacterMenu = GetCharacterMenu();
	if (CharacterMenu)
	{
		CharacterMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInGameOverlayWidget::HideCharacterMenu()
{
	UCharacterMenuWidget* CharacterMenu = GetCharacterMenu();
	if (CharacterMenu)
	{
		CharacterMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UInGameOverlayWidget::IsCharacterMenuVisible()
{
	bool IsCharacterMenuVisible = false;

	UCharacterMenuWidget* CharacterMenu = GetCharacterMenu();
	if (CharacterMenu)
	{
		IsCharacterMenuVisible = CharacterMenu->IsVisible();
	}

	return IsCharacterMenuVisible;
}
