// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactable.h"
#include "InteractPrompt.generated.h"

class AInteractable;

/**
 * UI Widget for on screen button prompts when focusing interactables.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInteractPrompt : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionText;

protected:
	UFUNCTION(BlueprintCallable)
	void OnInteractableFocused(AInteractable* Interactable);

	UFUNCTION(BlueprintCallable)
	void OnInteractableUnfocused();
};
