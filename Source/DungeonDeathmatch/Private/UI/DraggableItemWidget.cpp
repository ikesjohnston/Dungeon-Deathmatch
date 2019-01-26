// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableItemWidget.h"
#include <CanvasPanel.h>
#include <Image.h>
#include <Button.h>
#include "DungeonGameInstance.h"
#include "DungeonPlayerController.h"
#include "DungeonHUD.h"
#include <SlateBlueprintLibrary.h>
#include "EquipmentEnums.h"
#include "Equippable.h"
#include <WidgetBlueprintLibrary.h>

UDraggableItemWidget::UDraggableItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UDraggableItemWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (ItemCanvas)
	{
		ItemCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (ItemSelectButton)
	{
		ItemSelectButton->SetVisibility(ESlateVisibility::Visible);
	}

	return Result;
}

void UDraggableItemWidget::InitializeDraggableItem(AItem* DraggableItem, bool IsEquipment /*= false*/, FInventoryGridPair InventoryGridLocation /*= FInventoryGridPair()*/)
{
	if (!ItemImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableItemWidget::InitializeDraggableItem - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

	Item = DraggableItem;
	GridLocation = InventoryGridLocation;

	ItemImage->SetBrushFromTexture(Item->GetIcon());
	ItemImage->SetBrushSize(Item->GetGridSizeVector());

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (IsEquipment)
		{
			AEquippable* Equippable = Cast<AEquippable>(Item);
			if (Equippable)
			{
				FInventoryGridPair EquipmentSlotSize;
			}
		}
		else
		{
			float GridSize = GameInstance->GetInventoryGridSlotSize();
			FVector2D RenderTranslation = FVector2D(GridSize * InventoryGridLocation.Column, GridSize * InventoryGridLocation.Row);
			ItemCanvas->SetRenderTranslation(RenderTranslation);
			ItemSelectButton->WidgetStyle.Normal.ImageSize = Item->GetGridSizeVector();
			ItemSelectButton->WidgetStyle.Hovered.ImageSize = Item->GetGridSizeVector();
			ItemSelectButton->WidgetStyle.Pressed.ImageSize = Item->GetGridSizeVector();
			ItemSelectButton->WidgetStyle.Disabled.ImageSize = Item->GetGridSizeVector();
		}
	}
}

AItem* UDraggableItemWidget::GetItem()
{
	return Item;
}

bool UDraggableItemWidget::IsReadyForDrag()
{
	return bIsReadyForDrag;
}

void UDraggableItemWidget::StartDragging()
{
	bIsReadyForDrag = false;

	ItemSelectButton->SetVisibility(ESlateVisibility::Collapsed);

	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		Controller->StartDraggingItem(this);
		Controller->FocusUIAndGame();
	}
}

void UDraggableItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Draw an item tooltip at the item origin
	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		float GridSize = GameInstance->GetInventoryGridSlotSize();
		FVector2D RenderTranslation = FVector2D(GridSize * GridLocation.Column, GridSize * GridLocation.Row);

		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			Controller->SetSelectedItem(this);
			ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
			if (HUD)
			{
				FVector2D PixelPosition;
				FVector2D ViewportPosition;
				USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), FVector2D(0, 0), PixelPosition, ViewportPosition);
				ViewportPosition += RenderTranslation;
				HUD->ShowTooltipAtLocation(ViewportPosition, Item);
			}
		}
	}
}

void UDraggableItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	// Stop any initializing hold drag and hide the item tooltip
	bIsReadyForDrag = false;
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		Controller->SetSelectedItem(nullptr);
		ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
		if (HUD)
		{
			HUD->HideTooltip();
		}
	}
}

FReply UDraggableItemWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//  Processing "DropInventoryItem" key presses here. Mouse leaves the widget during mouse input capture, so can't reliably maintain a focus
 	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
 	if (Controller)
 	{
 		TArray<FInputActionKeyMapping> ActionKeys = Controller->GetKeyForAction(FName("DropInventoryItem"));
 		if (ActionKeys.Num() > 0)
 		{
 			FKey InventoryDropKey = ActionKeys[0].Key;
			if (InMouseEvent.GetEffectingButton() == InventoryDropKey)
			{
				Controller->OnDropInventoryItemKeyPressed();
			}
 		}
 	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// Initialize a possible hold item drag operation
		bIsReadyForDrag = true;

		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return FReply::Handled();
}

void UDraggableItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	OutOperation = nullptr;

	//  Processing "DropInventoryItem" key presses here. Mouse leaves the widget during mouse input capture, so can't reliably maintain a focus
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		// Shouldn't be able to start a hold drag if already dragging an item
		if (!Controller->GetDraggedItem())
		{
			// Just make a call to the controller to start dragging the item, no need to create a normal drag and drop operation class
			StartDragging();
		}
	}

}
