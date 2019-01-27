// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableItemWidget.h"
#include <CanvasPanel.h>
#include <Image.h>
#include <Button.h>
#include "DungeonGameInstance.h"
#include "DungeonPlayerController.h"
#include "DungeonHUD.h"
#include <SlateBlueprintLibrary.h>
#include "EquipmentGlobals.h"
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

void UDraggableItemWidget::InitializeDraggableItem(AItem* DraggableItem, FInventoryGridPair InventoryGridLocation /*= FInventoryGridPair()*/)
{
	if (!ItemImage || !ItemSelectButton)
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
		float GridSize = GameInstance->GetInventoryGridSlotSize();
		FVector2D RenderTranslation = FVector2D(GridSize * InventoryGridLocation.Column, GridSize * InventoryGridLocation.Row);
		ItemCanvas->SetRenderTranslation(RenderTranslation);
		ItemSelectButton->WidgetStyle.Normal.ImageSize = Item->GetGridSizeVector();
		ItemSelectButton->WidgetStyle.Hovered.ImageSize = Item->GetGridSizeVector();
		ItemSelectButton->WidgetStyle.Pressed.ImageSize = Item->GetGridSizeVector();
		ItemSelectButton->WidgetStyle.Disabled.ImageSize = Item->GetGridSizeVector();
	}
}

void UDraggableItemWidget::InitializeDraggableEquipment(AItem* DraggableItem, FInventoryGridPair SlotSize)
{
	if (!ItemImage || !ItemSelectButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableItemWidget::InitializeDraggableEquipment - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

	Item = DraggableItem;

	ItemImage->SetBrushFromTexture(Item->GetIcon());
	ItemImage->SetBrushSize(Item->GetGridSizeVector());

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		float GridSize = GameInstance->GetInventoryGridSlotSize();
		FVector2D SlotVectorSize = FVector2D(GridSize * SlotSize.Column, GridSize * SlotSize.Row);
		ItemSelectButton->WidgetStyle.Normal.ImageSize = SlotVectorSize;
		ItemSelectButton->WidgetStyle.Hovered.ImageSize = SlotVectorSize;
		ItemSelectButton->WidgetStyle.Pressed.ImageSize = SlotVectorSize;
		ItemSelectButton->WidgetStyle.Disabled.ImageSize = SlotVectorSize;
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

void UDraggableItemWidget::SetIsReadyForDrag(bool IsReadyForDrag)
{
	bIsReadyForDrag = IsReadyForDrag;
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
