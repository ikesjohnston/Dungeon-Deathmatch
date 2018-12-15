// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsMenuWidget.generated.h"

class ADungeonCharacter;

/**
 * Widget class used for displaying player stats. This widget also provides
 * controls for increasing stats when a character levels up.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UStatsMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	ADungeonCharacter* Character;

public:
	UFUNCTION(BlueprintCallable)
	void SetCharacter(ADungeonCharacter* NewCharacter);

};
