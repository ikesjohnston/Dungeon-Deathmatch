// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonHUD.h"
#include "UI/InGameOverlayWidget.h"

void ADungeonHUD::BeginPlay()
{
	if (InGameOverlayClass)
	{
		InGameOverlay = Cast<UInGameOverlayWidget>(CreateWidget(PlayerOwner, InGameOverlayClass, TEXT("In Game Overlay")));
		if (InGameOverlay)
		{
			InGameOverlay->AddToViewport();
			HideCharacterMenu();
		}
	}
}

UInGameOverlayWidget* ADungeonHUD::GetInGameOverlay()
{
	return InGameOverlay;
}

void ADungeonHUD::ShowCharacterMenu()
{
	if (InGameOverlay)
	{
		InGameOverlay->ShowCharacterMenu();
	}
}

void ADungeonHUD::HideCharacterMenu()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideCharacterMenu();
	}
}

bool ADungeonHUD::IsCharacterMenuVisible()
{
	if (InGameOverlay)
	{
		return InGameOverlay->IsCharacterMenuVisible();
	}
	return false;
}
