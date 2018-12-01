// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameOverlay.h"
#include "UI/InventoryMenu.h"
#include "UI/EquipmentMenu.h"
#include "Blueprint/WidgetTree.h"

UInventoryMenu* UInGameOverlay::GetInventoryMenu()
{
	UInventoryMenu* InventoryMenu = Cast<UInventoryMenu>(WidgetTree->FindWidget(InventoryMenuWidgetName));
	if (!InventoryMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::GetInventoryMenu - No InventoryMenu widget found in %s. Verify that InventoryMenuWidgetName is correctly set."), *GetName());
	}
	return InventoryMenu;
}

void UInGameOverlay::ShowInventoryMenu()
{
	UInventoryMenu* InventoryMenu = GetInventoryMenu();
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInGameOverlay::HideInventoryMenu()
{
	UInventoryMenu* InventoryMenu = GetInventoryMenu();
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}

UEquipmentMenu* UInGameOverlay::GetEquipmentMenu()
{
	UEquipmentMenu* EquipmentMenu = Cast<UEquipmentMenu>(WidgetTree->FindWidget(EquipmentMenuWidgetName));
	if (!EquipmentMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::GetEquipmentMenu - No EquipmentMenu widget found in %s. Verify that EquipmentMenuWidgetName is correctly set."), *GetName());
	}
	return EquipmentMenu;
}

void UInGameOverlay::ShowEquipmentMenu()
{
	UEquipmentMenu* EquipmentMenu = GetEquipmentMenu();
	if (EquipmentMenu)
	{
		EquipmentMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInGameOverlay::HideEquipmentMenu()
{
	UEquipmentMenu* EquipmentMenu = GetEquipmentMenu();
	if (EquipmentMenu)
	{
		EquipmentMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UInGameOverlay::AreInventoryAndEquipmentMenusVisible()
{
	bool IsInventoryVisible = false;
	bool IsEquipmentVisible = false;

	UInventoryMenu* InventoryMenu = GetInventoryMenu();
	if (InventoryMenu)
	{
		IsInventoryVisible = InventoryMenu->IsVisible();
	}

	UEquipmentMenu* EquipmentMenu = GetEquipmentMenu();
	if (EquipmentMenu)
	{
		IsEquipmentVisible = EquipmentMenu->IsVisible();
	}

	return IsInventoryVisible && IsEquipmentVisible;
}
