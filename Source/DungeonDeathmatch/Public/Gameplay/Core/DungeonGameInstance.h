// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Engine/StreamableManager.h>
#include "DungeonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	FStreamableManager AssetLoader;

public:
	UDungeonGameInstance(const FObjectInitializer& ObjectInitializer);

	FStreamableManager& GetAssetLoader();
};
