// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"

class UImage;
class UButton;
class UCharacterMenuWidget;
class UInventoryMenuWidget;
class AItem;
class UDragAndDropItemWidget;

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

public:
	virtual bool Initialize() override;

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
	bool IsCharacterMenuVisible();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowReticle();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideReticle();

	/** Renders an item icon over the cursor and activates the item drop screen area */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartDragAndDropOperation(AItem* Item);

	/** Stops rendering an item icon over the cursor and deactivates the item drop screen area */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StopDragAndDropOperation();

	UFUNCTION()
	void OnDropItemScreenButtonPressed();
};
