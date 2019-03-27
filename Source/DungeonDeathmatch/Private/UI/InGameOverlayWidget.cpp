// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameOverlayWidget.h"
#include "CharacterMenuWidget.h"
#include "InventoryMenuWidget.h"
#include "InventoryComponent.h"
#include "DungeonCharacter.h"
#include "DungeonPlayerController.h"
#include "DraggableItemWidget.h"
#include "DragAndDropItemWidget.h"
#include "Item.h"
#include "CharacterRenderCapture2D.h"

#include <Image.h>
#include <Button.h>
#include <BackgroundBlur.h>

UInGameOverlayWidget::UInGameOverlayWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	GameBackgroundBlurStrength = 50.0f;
	GameBackgroundBlurTime = 0.25f;
}

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
		DropItemScreenButton->OnHovered.AddDynamic(this, &UInGameOverlayWidget::OnDropItemScreenButtonHovered);
		DropItemScreenButton->OnUnhovered.AddDynamic(this, &UInGameOverlayWidget::OnDropItemScreenButtonUnhovered);
		DropItemScreenButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (GameBackgroundBlur)
	{
		GameBackgroundBlur->SetBlurStrength(0);
	}

	return Result;
}

void UInGameOverlayWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (GameBackgroundBlur)
	{
		if (bIsBluringBackground && GameBackgroundBlur->BlurStrength < GameBackgroundBlurStrength)
		{
			GameBackgroundBlur->SetBlurStrength(GameBackgroundBlur->BlurStrength + (GameBackgroundBlurStrength * (DeltaTime / GameBackgroundBlurTime)));
		}
		else if (!bIsBluringBackground && GameBackgroundBlur->BlurStrength > 0)
		{
			GameBackgroundBlur->SetBlurStrength(GameBackgroundBlur->BlurStrength - (GameBackgroundBlurStrength * (DeltaTime / GameBackgroundBlurTime)));
		}
	}

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

bool UInGameOverlayWidget::AreMenusOpen()
{
	if (!CharacterMenu || !InventoryMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlay::AreMenusOpen - No Menu widgets found in %s. Verify that widgets are correctly set."), *GetName());
	}

	if (CharacterMenu->IsVisible() || InventoryMenu->IsVisible())
	{
		return true;
	}

	return false;
}

void UInGameOverlayWidget::ShowReticle()
{
	ReticleImage->SetVisibility(ESlateVisibility::Visible);
}

void UInGameOverlayWidget::HideReticle()
{
	ReticleImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UInGameOverlayWidget::ShowTooltipAtLocation(FVector2D ScreenLocation, AItem* Item)
{
	if (HoveredItemTooltip)
	{
		HoveredItemTooltip->SetRenderTranslation(ScreenLocation);
		HoveredItemTooltip->SetItem(Item);
		HoveredItemTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::ShowTooltipAtLocation - No HoveredItemTooltip widget found for %s."), *GetName());
	}
}

void UInGameOverlayWidget::HideTooltip()
{
	if (HoveredItemTooltip)
	{
		HoveredItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::HideTooltip - No HoveredItemTooltip widget found for %s."), *GetName());
	}
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

void UInGameOverlayWidget::StopDragAndDropOperation(bool WasCanceled)
{
	DropItemScreenButton->SetVisibility(ESlateVisibility::Collapsed);

	if (WasCanceled)
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
//					Character->ServerRequestDropItem(DraggedItemWidget->GetItem(), false);
				}
			}
		}
	}

	if (DragAndDropItem)
	{
		DragAndDropItem->StopDragAndDropOperation();	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInGameOverlayWidget::StopDragAndDropOperation - No DragAndDropItem widget found for %s."), *GetName());
	}
}

void UInGameOverlayWidget::CheckForItemDrop()
{
	ADungeonPlayerController* PlayerController = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		UDraggableItemWidget* DraggedItemWidget = PlayerController->GetDraggedItem();
		if (DraggedItemWidget && DraggedItemWidget->GetItem())
		{
			UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(PlayerController->GetPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
			if (InventoryComponent)
			{
				AItem* DraggedItem = DraggedItemWidget->GetItem();
				InventoryComponent->ServerRequestDropItem(DraggedItem);
				UGameplayStatics::PlaySound2D(DraggedItem->GetWorld(), DraggedItem->GetInteractionSound());
			}
			PlayerController->StopDraggingItem(true);
		}
	}
}

void UInGameOverlayWidget::BlurBackground()
{
	bIsBluringBackground = true;
}

void UInGameOverlayWidget::UnblurBackground()
{
	bIsBluringBackground = false;
}

void UInGameOverlayWidget::OnDropItemScreenButtonPressed()
{
	CheckForItemDrop();
}

void UInGameOverlayWidget::OnDropItemScreenButtonHovered()
{
	bIsHoveringDropArea = true;
}

void UInGameOverlayWidget::OnDropItemScreenButtonUnhovered()
{
	bIsHoveringDropArea = false;
}

FReply UInGameOverlayWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bIsHoveringDropArea)
		{
			CheckForItemDrop();
		}

		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			Controller->SetSelectedRenderCaptureActor(nullptr);
		}
	}

	return FReply::Handled();
}

FReply UInGameOverlayWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		ACharacterRenderCapture2D* SelectedRenderCaptureActor = Controller->GetSelectedRenderCaptureActor();
		if (SelectedRenderCaptureActor)
		{
			FVector2D MouseDelta = InMouseEvent.GetCursorDelta();
			SelectedRenderCaptureActor->AddToTargetRotation(-MouseDelta.X);
		}
	}

	return FReply::Handled();
}
