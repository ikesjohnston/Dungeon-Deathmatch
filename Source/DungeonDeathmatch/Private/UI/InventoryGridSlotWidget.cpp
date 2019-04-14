// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryGridSlotWidget.h"
#include "Item.h"

#include <Image.h>

bool UInventoryGridSlotWidget::Initialize()
{
	bool Result = Super::Initialize();
	if (!ValidateWidgets())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridSlotWidget::Initialize - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());
		Result = false;
	}
	else
	{
		SlotBackground->SetColorAndOpacity(DefaultBackgroundColor);
		SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
	}
	return Result;
}

void UInventoryGridSlotWidget::SetItem(AItem* NewItem)
{
	if (!ValidateWidgets()) return;
	Item = NewItem;
	if (Item)
	{
		SlotBackground->SetColorAndOpacity(Item->GetQualityTierColor());
	}
	else
	{
		SlotBackground->SetColorAndOpacity(DefaultBackgroundColor);
	}
}

void UInventoryGridSlotWidget::BeginItemOverlap(bool IsOverlapValid)
{
	if (!ValidateWidgets()) return;
	SlotHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (IsOverlapValid)
	{
		SlotHighlight->SetColorAndOpacity(ValidOverlapHighlightColor);
	}
	else
	{
		SlotHighlight->SetColorAndOpacity(InvalidOverlapHighlightColor);
	}
}

void UInventoryGridSlotWidget::EndItemOverlap()
{
	if (!ValidateWidgets()) return;
	SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
}

bool UInventoryGridSlotWidget::ValidateWidgets()
{
	bool Result = false;
	if (SlotBackground && SlotHighlight)
	{
		Result = true;
	}
	return Result;
}
