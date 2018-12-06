// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactable.h"
#include "InteractTooltip.generated.h"

class AInteractable;
class UInteractTooltip;

/**
 * UI Widget for on screen button prompts when focusing interactables.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInteractTooltip : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	AInteractable* Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionText;

	/** Separate child widget for items. Hidden by default, will be set visible by parent item. */
	UItemTooltip* ItemTooltip;

	/** The name given to the ItemTooltip widget in the  editor. Used to find the widget in code. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FName ItemTooltipWidgetName;

public:
	UFUNCTION(BlueprintCallable)
	void SetInteractable(AInteractable* NewInteractable);

	UFUNCTION(BlueprintCallable)
	UItemTooltip* GetItemTooltip();

protected:
	UFUNCTION(BlueprintCallable)
	void OnInteractableFocused();

	UFUNCTION(BlueprintCallable)
	void OnInteractableUnfocused();

	/** Initialized ItemTooltip widget to be hidden */
	void InitItemTooltip();
};
