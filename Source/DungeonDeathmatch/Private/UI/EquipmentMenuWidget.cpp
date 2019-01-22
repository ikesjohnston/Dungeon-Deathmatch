// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentMenuWidget.h"

UEquipmentMenuWidget::UEquipmentMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UEquipmentMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	return Result;
}

bool UEquipmentMenuWidget::TryEquipItem(AEquippable* Equippable)
{
	bool Result = false;

	return Result;
}

void UEquipmentMenuWidget::UnequipItem(AEquippable* Equippable)
{

}
