// Fill out your copyright notice in the Description page of Project Settings.

#include "InventorySlotWidget.h"

bool UInventorySlotWidget::IsOccupied()
{
	if (Item)
	{
		return true;
	}
	return false;
}

AItem* UInventorySlotWidget::GetItem()
{
	return Item;
}

void UInventorySlotWidget::SetItem(AItem* NewItem)
{
	Item = NewItem;
}
