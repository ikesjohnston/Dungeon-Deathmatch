// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEquipmentSlot.h"

bool UInventoryEquipmentSlot::IsOccupied()
{
	if (Item)
	{
		return true;
	}
	return false;
}
