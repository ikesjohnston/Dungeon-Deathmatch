// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonHUD.h"
#include "UI/InGameOverlay.h"

void ADungeonHUD::BeginPlay()
{
	if (InGameOverlayClass)
	{
		InGameOverlay = Cast<UInGameOverlay>(CreateWidget(PlayerOwner, InGameOverlayClass, TEXT("In Game Overlay")));
		if (InGameOverlay)
		{
			InGameOverlay->AddToViewport();
			HideInventoryAndEquipmentMenus();
		}
	}
}

UInGameOverlay* ADungeonHUD::GetInGameOverlay()
{
	return InGameOverlay;
}

void ADungeonHUD::ShowInventoryAndEquipmentMenus()
{
	if (InGameOverlay)
	{
		InGameOverlay->ShowInventoryMenu();
		InGameOverlay->ShowEquipmentMenu();
	}
}

void ADungeonHUD::HideInventoryAndEquipmentMenus()
{
	if (InGameOverlay)
	{
		InGameOverlay->HideInventoryMenu();
		InGameOverlay->HideEquipmentMenu();
	}
}

bool ADungeonHUD::AreInventoryAndEquipmentMenusVisible()
{
	if (InGameOverlay)
	{
		return InGameOverlay->AreInventoryAndEquipmentMenusVisible();
	}
	return false;
}
