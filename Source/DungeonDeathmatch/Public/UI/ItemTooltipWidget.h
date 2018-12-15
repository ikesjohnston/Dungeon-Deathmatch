// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class AItem;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	AItem* Item;

public:
	UFUNCTION(BlueprintCallable)
	void SetItem(AItem* NewItem);
};
