// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include <Sound/SoundCue.h>
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	USoundCue* InventoryOpenSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	USoundCue* InventoryCloseSound;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInGameOverlayWidget* GetInGameOverlay();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool AreMenusVisible();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowReticle();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideReticle();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowTooltipAtLocation(FVector2D ScreenLocation, AItem* Item);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideTooltip();

	void StartDragAndDropOperation(AItem* Item);

	void StopDragAndDropOperation(bool WasCanceled);

	// Blurs the game view, used for when menus are opened
	void BlurBackground();

	// Unblurs the game view, used for when menus are closed
	void UnblurBackground();
};
