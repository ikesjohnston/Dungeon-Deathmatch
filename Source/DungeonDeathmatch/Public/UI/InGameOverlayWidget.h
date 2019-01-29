// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.h"
#include "InGameOverlayWidget.generated.h"

class UImage;
class UButton;
class UCharacterMenuWidget;
class UInventoryMenuWidget;
class AItem;
class UDragAndDropItemWidget;
class UBackgroundBlur;

/**
 * The main UI widget that contains all other menu widgets that the player can see while in a game.
 * This should be the only widget directly drawn to the viewport, and all other widgets will be accessed from here.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The character menu widget that displays character stats */
	UPROPERTY(meta = (BindWidget))
	UCharacterMenuWidget* CharacterMenu;

	/** The inventory menu widget that displays the character inventory and equipment */
	UPROPERTY(meta = (BindWidget))
	UInventoryMenuWidget* InventoryMenu;

	/** The drag and drop item operation display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UDragAndDropItemWidget* DragAndDropItem;

	/** The image used for the aiming reticle during gameplay */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UImage* ReticleImage;

	/** The button taking up all screen space not already covered in UI elements, for dropping items that are being dragged on click */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UButton* DropItemScreenButton;

	/** The background blur applied when menus are visible */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UBackgroundBlur* GameBackgroundBlur;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget, ClampMin = 0.0f, ClampMax = 100.0f))
	float GameBackgroundBlurStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget, ClampMin = 0.0f, ClampMax = 1.0f))
	float GameBackgroundBlurTime;

	/** The tooltip widget for the currently hovered item, if any */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UItemTooltipWidget* HoveredItemTooltip;

private:
	bool bIsBluringBackground;

	bool bIsHoveringDropArea;

public:
	UInGameOverlayWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UCharacterMenuWidget* GetCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideCharacterMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryMenuWidget* GetInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool AreMenusOpen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowReticle();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideReticle();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowTooltipAtLocation(FVector2D ScreenLocation, AItem* Item);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideTooltip();

	/** Renders an item icon over the cursor and activates the item drop screen area */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartDragAndDropOperation(AItem* Item);

	/** Stops rendering an item icon over the cursor and deactivates the item drop screen area */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StopDragAndDropOperation(bool WasCanceled);

	UFUNCTION()
	void CheckForItemDrop();

	UFUNCTION()
	void BlurBackground();

	UFUNCTION()
	void UnblurBackground();

	UFUNCTION()
	void OnDropItemScreenButtonPressed();

	UFUNCTION()
	void OnDropItemScreenButtonHovered();

	UFUNCTION()
	void OnDropItemScreenButtonUnhovered();

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
