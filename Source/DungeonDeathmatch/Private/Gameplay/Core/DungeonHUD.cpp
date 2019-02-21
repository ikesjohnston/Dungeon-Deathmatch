// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonHUD.h"
#include "UI/InGameOverlayWidget.h"
#include <Kismet/GameplayStatics.h>

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
		UGameplayStatics::PlaySound2D(GetWorld(), InventoryOpenSound);
	}
}

void ADungeonHUD::HideInventoryMenu()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideInventoryMenu();
		UGameplayStatics::PlaySound2D(GetWorld(), InventoryCloseSound);
	}
}

void ADungeonHUD::ShowReticle()
{
	if (InGameOverlay)
	{
		InGameOverlay->ShowReticle();
	}
}

void ADungeonHUD::HideReticle()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideReticle();
	}
}

void ADungeonHUD::ShowTooltipAtLocation(FVector2D ScreenLocation, AItem* Item)
{
	if (InGameOverlay)
	{
		InGameOverlay->ShowTooltipAtLocation(ScreenLocation, Item);
	}
}

void ADungeonHUD::HideTooltip()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideTooltip();
	}
}

void ADungeonHUD::StartDragAndDropOperation(AItem* Item)
{
	if (InGameOverlay)
	{
		InGameOverlay->StartDragAndDropOperation(Item);
	}
}

void ADungeonHUD::StopDragAndDropOperation(bool WasCanceled)
{
	if (InGameOverlay)
	{
		InGameOverlay->StopDragAndDropOperation(WasCanceled);
	}
}

void ADungeonHUD::BlurBackground()
{
	if (InGameOverlay)
	{
		InGameOverlay->BlurBackground();
	}
}

void ADungeonHUD::UnblurBackground()
{
	if (InGameOverlay)
	{
		InGameOverlay->UnblurBackground();
	}
}
