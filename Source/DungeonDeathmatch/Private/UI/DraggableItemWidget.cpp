// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableItemWidget.h"
#include <CanvasPanel.h>
#include <Image.h>
#include <Border.h>
#include <Button.h>
#include "DungeonGameInstance.h"
#include "DungeonPlayerController.h"
#include "ItemTooltipWidget.h"

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

	/*if (DraggingTintImage)
	{
		DraggingTintImage->SetVisibility(ESlateVisibility::Collapsed);
	}*/
	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ItemSelectButton)
	{
		ItemSelectButton->SetVisibility(ESlateVisibility::Visible);
		ItemSelectButton->OnHovered.AddDynamic(this, &UDraggableItemWidget::OnItemSelectButtonHovered);
		ItemSelectButton->OnUnhovered.AddDynamic(this, &UDraggableItemWidget::OnItemSelectButtonUnhovered);

		ItemSelectButton->OnClicked.AddDynamic(this, &UDraggableItemWidget::OnItemSelectButtonPressed);
		ItemSelectButton->OnReleased.AddDynamic(this, &UDraggableItemWidget::OnItemSelectButtonReleased);
	}
	BindToController();

	return Result;
}

AItem* UDraggableItemWidget::GetItem()
{
	return Item;
}

void UDraggableItemWidget::SetItem(AItem* NewItem)
{
	if (!ItemImage || /*!ItemBorder ||*/ !ItemTooltip)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableItemWidget::SetItem - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

	Item = NewItem;


	ItemImage->SetBrushFromTexture(Item->GetIcon());
	ItemImage->SetBrushSize(Item->GetGridSizeVector());

	/*UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		ItemBorder->SetContentColorAndOpacity(Item->GetQualityTierColor());
		ItemBorder->SetBrushColor(Item->GetQualityTierTextColor());
	}*/

	ItemTooltip->SetItem(Item);
}

void UDraggableItemWidget::SetItemGridLocation(FInventoryGridPair NewGridLocation)
{
	if (!ItemImage || !ItemTooltip)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableItemWidget::SetItemGridLocation - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

	GridLocation = NewGridLocation;

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		float GridSize = GameInstance->GetInventoryGridSlotSize();
		FVector2D RenderTranslation = FVector2D(GridSize * GridLocation.Column, GridSize * GridLocation.Row);
		ItemCanvas->SetRenderTranslation(RenderTranslation);
		ItemTooltip->SetRenderTranslation(RenderTranslation);
	}
}

void UDraggableItemWidget::BindToController()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		Controller->OnBeginItemDrag.AddDynamic(this, &UDraggableItemWidget::OnBeginItemDrag);
		Controller->OnEndItemDrag.AddDynamic(this, &UDraggableItemWidget::OnEndItemDrag);
	}
}

void UDraggableItemWidget::OnItemSelectButtonHovered()
{
	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDraggableItemWidget::OnItemSelectButtonUnhovered()
{
	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDraggableItemWidget::OnItemSelectButtonPressed()
{
	ItemSelectButton->SetVisibility(ESlateVisibility::Collapsed);

	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}

	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		Controller->StartDraggingItem(this);
		Controller->FocusUIAndGame();
	}
}

void UDraggableItemWidget::OnItemSelectButtonReleased()
{
	ItemSelectButton->SetVisibility(ESlateVisibility::Visible);

	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		Controller->FocusUIAndGame();
	}
}

void UDraggableItemWidget::OnBeginItemDrag(AItem* DraggedItem)
{
	if (DraggedItem == Item)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDraggableItemWidget::OnEndItemDrag(AItem* DraggedItem)
{
	if (DraggedItem == Item)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}
