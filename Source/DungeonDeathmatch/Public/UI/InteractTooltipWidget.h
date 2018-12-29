// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactable.h"
#include "InteractTooltipWidget.generated.h"

class AInteractableActor;
class UItemTooltipWidget;

/**
 * UI Widget for on screen button prompts when focusing interactables.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInteractTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/* The interactable actor this tooltip details. It is assumed that this actor implements IInteractable. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	AActor* Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionText;

	/** Separate child widget for items. Hidden by default, will be set visible by parent item. */
	UItemTooltipWidget* ItemTooltip;

	/** The name given to the ItemTooltip widget in the  editor. Used to find the widget in code. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	FName ItemTooltipWidgetName;

public:
	UFUNCTION(BlueprintCallable)
	void SetInteractable(AActor* NewInteractable);

	UFUNCTION(BlueprintCallable)
	UItemTooltipWidget* GetItemTooltip();

protected:
	UFUNCTION(BlueprintCallable)
	void OnInteractableFocused();

	UFUNCTION(BlueprintCallable)
	void OnInteractableUnfocused();

	/** Initialized ItemTooltip widget to be hidden */
	void InitItemTooltip();
};
