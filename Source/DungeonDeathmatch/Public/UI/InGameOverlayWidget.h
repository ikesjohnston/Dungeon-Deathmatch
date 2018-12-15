// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"

class UCharacterMenuWidget;

/**
 * The main UI widget that contains all other menu widgets that the player can see while in a game.
 * This should be the only widget directly drawn to the viewport, and all other widgets will be accessed from here.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** 
	 * The name of the CharacterMenuWidget that is contained in this widget in the editor. 
	 * Used to find the widget from the WidgetTree.
	 */
	UPROPERTY(EditAnywhere, Category = "UI")
	FName CharacterMenuWidgetName;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UCharacterMenuWidget* GetCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsCharacterMenuVisible();
};
