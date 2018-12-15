// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameMode.h"
#include "DungeonGameState.h"
#include "DungeonPlayerState.h"
#include "DungeonCharacter.h"
#include "DungeonHUD.h"


ADungeonGameMode::ADungeonGameMode()
{
	DefaultPawnClass = ADungeonCharacter::StaticClass();
	GameStateClass = ADungeonGameState::StaticClass();
	PlayerStateClass = ADungeonPlayerState::StaticClass();
	HUDClass = ADungeonHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ADungeonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

TMap<EItemQualityTier, FLinearColor> ADungeonGameMode::GetItemQualityTierColors()
{
	return ItemQualityTierColors;
}

TMap<EEquipmentSlot, UTexture2D*> ADungeonGameMode::GetEmptyEquipmentSlotIcons()
{
	return EmptyEquipmentSlotIcons;
}

FVector ADungeonGameMode::GetRandomLootEjectionForce()
{
	FVector EjectionVector = FVector(LootEjectionForwardForce, 0, LootEjectionUpForce);
	float RandomForwardAngle = FMath::FRand() * 360.0f;
	EjectionVector.RotateAngleAxis(RandomForwardAngle, FVector::UpVector);

	return EjectionVector;
}

void ADungeonGameMode::GameOver()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over!"));
}

void ADungeonGameMode::StartPlay()
{
	Super::StartPlay();
}
