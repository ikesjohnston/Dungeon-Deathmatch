// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DungeonHUD.generated.h"

class UInGameOverlayWidget;

/**
 * HUD class for toggling the display of different UI widgets and screens. Can be accessed from the PlayerController. 
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInGameOverlayWidget> InGameOverlayClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UInGameOverlayWidget* InGameOverlay;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInGameOverlayWidget* GetInGameOverlay();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsCharacterMenuVisible();

};
