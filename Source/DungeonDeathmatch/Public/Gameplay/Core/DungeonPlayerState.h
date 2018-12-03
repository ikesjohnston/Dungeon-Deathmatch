// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DungeonPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float DeltaScore);
	
};
