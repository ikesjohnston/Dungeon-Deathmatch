// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

#include <ConstructorHelpers.h>
#include <UserWidget.h>
#include "MainMenuWidget.h"
#include "InGameMenuWidget.h"
#include "DungeonMenuWidget.h"

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InventoryGridSlotSize = 40.0f;

	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClass(TEXT("/Game/UI/Menus/WBP_MainMenu"));
	MainMenuWidgetClass = MainMenuClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClass(TEXT("/Game/UI/Menus/WBP_InGameMenu"));
	InGameMenuWidgetClass = InGameMenuClass.Class;
}

void UDungeonGameInstance::Init()
{
	if (MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Found main menu class %s"), *MainMenuWidgetClass->GetName());
	}

	if (InGameMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonGameInstance::Init - Found in game menu class %s"), *InGameMenuWidgetClass->GetName());
	}
}

void UDungeonGameInstance::LoadMainMenu()
{
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UDungeonMenuWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->SetMenuInterface(this);
			MainMenuWidget->Setup();
		}
	}
}

void UDungeonGameInstance::LoadInGameMenu()
{
	if (InGameMenuWidgetClass)
	{
		InGameMenuWidget = CreateWidget<UDungeonMenuWidget>(this, InGameMenuWidgetClass);
		if (InGameMenuWidget)
		{
			InGameMenuWidget->SetMenuInterface(this);
			InGameMenuWidget->Setup();
		}
	}
}

void UDungeonGameInstance::HostGame()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (Engine)
	{
		Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Levels/TraversalTestMap?listen");
	}
}

void UDungeonGameInstance::JoinGame(const FString& Address)
{
	if (MainMenuWidget)
	{
		MainMenuWidget->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (Engine)
	{
		Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UDungeonGameInstance::ExitToMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ClientTravel("/Game/Levels/MainMenuMap", ETravelType::TRAVEL_Absolute);
	}
}

void UDungeonGameInstance::ExitToDesktop()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController(GetWorld());
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierColors()
{
	return ItemQualityTierColors;
}

TMap<EItemQualityTier, FLinearColor> UDungeonGameInstance::GetItemQualityTierTextColors()
{
	return ItemQualityTierTextColors;
}

TSubclassOf<UDraggableItemWidget> UDungeonGameInstance::GetDragAndDropItemWidgetClass()
{
	return DragAndDropItemWidgetClass;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetSheatheAnimationMontages()
{
	return SheatheAnimationMontages;
}

TMap<EWeaponSocketType, UAnimMontage*> UDungeonGameInstance::GetUnsheatheAnimationMontages()
{
	return UnsheatheAnimationMontages;
}

float UDungeonGameInstance::GetInventoryGridSlotSize()
{
	return InventoryGridSlotSize;
}

FVector UDungeonGameInstance::GetRandomLootEjectionForce()
{
	// Get a random forward ejection vector
	FVector EjectionVector = FVector(LootEjectionForwardForce, 0, LootEjectionUpForce);
	float RandomForwardAngle = FMath::FRand() * 360.0f;
	EjectionVector.RotateAngleAxis(RandomForwardAngle, FVector::UpVector);

	return EjectionVector;
}
