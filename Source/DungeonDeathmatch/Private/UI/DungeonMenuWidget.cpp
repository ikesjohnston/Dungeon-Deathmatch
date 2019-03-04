// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonMenuWidget.h"

bool UDungeonMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	return Result;
}

void UDungeonMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Teardown();
}

void UDungeonMenuWidget::SetMenuInterface(IMenuInterface* MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

void UDungeonMenuWidget::Setup()
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

void UDungeonMenuWidget::Teardown()
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