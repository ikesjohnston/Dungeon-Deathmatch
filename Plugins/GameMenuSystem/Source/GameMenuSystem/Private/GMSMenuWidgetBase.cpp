// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSMenuWidgetBase.h"

bool UGMSMenuWidgetBase::Initialize()
{
	bool Result = Super::Initialize();

	return Result;
}

void UGMSMenuWidgetBase::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Teardown();
}

void UGMSMenuWidgetBase::SetMenuInterface(IGMSMenuInterface* MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

void UGMSMenuWidgetBase::Setup()
{
	AddToViewport();

	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void UGMSMenuWidgetBase::Teardown()
{
	RemoveFromViewport();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = false;
		}
	}
}