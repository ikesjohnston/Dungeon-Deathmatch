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
			ShowReticle();
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

bool ADungeonHUD::AreMenusVisible()
{
	if (InGameOverlay)
	{
		return InGameOverlay->AreMenusOpen();
	}
	return false;
}

void ADungeonHUD::ShowInventoryMenu()
{
	if (InGameOverlay)
	{
		InGameOverlay->ShowInventoryMenu();
	}
}

void ADungeonHUD::HideInventoryMenu()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideInventoryMenu();
	}
}

void ADungeonHUD::ShowReticle()
{
	InGameOverlay->ShowReticle();
}

void ADungeonHUD::HideReticle()
{
	InGameOverlay->HideReticle();
}

void ADungeonHUD::ShowTooltipAtLocation(FVector2D ScreenLocation, AItem* Item)
{
	InGameOverlay->ShowTooltipAtLocation(ScreenLocation, Item);
}

void ADungeonHUD::HideTooltip()
{
	InGameOverlay->HideTooltip();
}

void ADungeonHUD::StartDragAndDropOperation(AItem* Item)
{
	InGameOverlay->StartDragAndDropOperation(Item);
}

void ADungeonHUD::StopDragAndDropOperation(bool WasCanceled)
{
	InGameOverlay->StopDragAndDropOperation(WasCanceled);
}

void ADungeonHUD::BlurBackground()
{
	InGameOverlay->BlurBackground();
}

void ADungeonHUD::UnblurBackground()
{
	InGameOverlay->UnblurBackground();
}
