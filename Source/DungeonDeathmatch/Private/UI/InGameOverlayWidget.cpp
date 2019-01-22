// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameOverlayWidget.h"
#include "CharacterMenuWidget.h"
#include "InventoryMenuWidget.h"
#include "DungeonCharacter.h"
#include "DungeonPlayerController.h"
#include <Image.h>
#include <Button.h>
#include "DraggableItemWidget.h"
#include "DragAndDropItemWidget.h"
#include "Item.h"

bool UInGameOverlayWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (CharacterMenu)
	{
		CharacterMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DropItemScreenButton)
	{
		DropItemScreenButton->OnPressed.AddDynamic(this, &UInGameOverlayWidget::OnDropItemScreenButtonPressed);
		DropItemScreenButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	return Result;
}

UCharacterMenuWidget* UInGameOverlayWidget::GetCharacterMenu()
{
	if (!CharacterMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::GetCharacterMenu - No CharacterMenu widget found in %s. Verify that CharacterMenu is correctly set."), *GetName());
	}
	return CharacterMenu;
}

void UInGameOverlayWidget::ShowCharacterMenu()
{
	if (CharacterMenu)
	{
		CharacterMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInGameOverlayWidget::HideCharacterMenu()
{
	if (CharacterMenu)
	{
		CharacterMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UInventoryMenuWidget* UInGameOverlayWidget::GetInventoryMenu()
{
	if (!InventoryMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::GetInventoryMenu - No InventoryMenu widget found in %s. Verify that InventoryMenu is correctly set."), *GetName());
	}
	return InventoryMenu;
}

void UInGameOverlayWidget::ShowInventoryMenu()
{
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInGameOverlayWidget::HideInventoryMenu()
{
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
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

void UInGameOverlayWidget::ShowReticle()
{
	ReticleImage->SetVisibility(ESlateVisibility::Visible);
}

void UInGameOverlayWidget::HideReticle()
{
	ReticleImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UInGameOverlayWidget::StartDragAndDropOperation(AItem* Item)
{
	DropItemScreenButton->SetVisibility(ESlateVisibility::Visible);
	if (DragAndDropItem)
	{
		DragAndDropItem->StartDragAndDropOperation(Item);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::StartDragAndDropOperation - No DragAndDropItem widget found for %s."), *GetName());
	}
}

void UInGameOverlayWidget::StopDragAndDropOperation()
{
	DropItemScreenButton->SetVisibility(ESlateVisibility::Collapsed);
	if (DragAndDropItem)
	{
		DragAndDropItem->StopDragAndDropOperation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::StopDragAndDropOperation - No DragAndDropItem widget found for %s."), *GetName());
	}
}

void UInGameOverlayWidget::OnDropItemScreenButtonPressed()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		UDraggableItemWidget* DraggedItemWidget = PlayerController->GetDraggedItem();
		if (DraggedItemWidget && DraggedItemWidget->GetItem())
		{
			ADungeonCharacter* Character = Cast<ADungeonCharacter>(PlayerController->GetPawn());
			if (Character)
			{
				Character->Server_TryDropItem(DraggedItemWidget->GetItem());
			}
			PlayerController->StopDraggingItem();
		}
	}
}
