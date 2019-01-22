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
