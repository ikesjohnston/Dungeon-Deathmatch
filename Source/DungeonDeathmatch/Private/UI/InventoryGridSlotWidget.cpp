// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryGridSlotWidget.h"
#include <Image.h>
#include "Item.h"

UInventoryGridSlotWidget::UInventoryGridSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UInventoryGridSlotWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (SlotBackground)
	{
		SlotBackground->SetColorAndOpacity(DefaultBackgroundColor);
	}
	if (SlotHighlight)
	{
		SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
	}

	return Result;
}

AItem* UInventoryGridSlotWidget::GetItem()
{
	return Item;
}

void UInventoryGridSlotWidget::SetItem(AItem* NewItem)
{
	if (!SlotBackground)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGridSlotWidget::SetItem - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

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
	if (SlotHighlight)
	{
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
}

void UInventoryGridSlotWidget::EndItemOverlap()
{
	if (SlotHighlight)
	{
		SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
	}
}
