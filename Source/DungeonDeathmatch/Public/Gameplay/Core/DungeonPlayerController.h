// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/PlayerController.h"
#include <GameFramework/PlayerInput.h>
#include "DungeonPlayerController.generated.h"

class UInteractable;
class AItem;
class UInventoryEquipmentSlotWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusedSignature, class UInteractable*, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableUnfocusedSignature);

/**
 * Controller class for the player, handles much of the UI and interaction logic.
 * Exists on the server and owning client.
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	/* Delegate called when interactable is focused by player. For UI updates. */
	FOnInteractableFocusedSignature OnInteractableFocused;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	/* Delegate called when interactable is unfocused by player. For UI updates. */
	FOnInteractableUnfocusedSignature OnInteractableUnfocused;

protected:
	/* The interactable actor that is currently focused by the player. It is assumed that this actor implements IInteractable. */
	AActor* FocusedInteractable;

	/* 
	 * The distance in front of the camera to start the trace for interactables. This attempts to prevent the trace from potentially hitting items behind the player when the 
	 * camera is being pushed forward from a collision with a wall or other object.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionCameraTraceForwardOffset;

	/* The distance from the camera to trace for interactables. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionCameraTraceDistance;

	/* The radius of the multi sphere trace from the camera for interactables. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionCameraTraceRadius;

	/* The max distance from the player that an interactable hit can be valid. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float MaxInteractionDistance;

	/* The InventoryEquipmentSlotWidget that the player is currently mousing over. */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UInventoryEquipmentSlotWidget* HoveringInventoryEquipmentSlot;

public:
	ADungeonPlayerController();

	virtual void Tick(float DeltaTime) override;

	/**
	 * Get the interactable actor that the player currently has focused, if any
	 */
	AActor* GetFocusedInteractable();

	/**
	 * Get the InventoryEquipmentSlot UI object that the player is currently hovering over with their cursor, if any
	 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryEquipmentSlotWidget* GetHoveringInventoryEquipmentSlot();

	/**
	 * Set the InventoryEquipmentSlot UI object that the player is currently hovering over with their cursor
	 * 
	 * @param InventoryEquipmentSlot The slot object being hovered
	 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHoveringInventoryEquipmientSlot(UInventoryEquipmentSlotWidget* InventoryEquipmentSlot);

	/**
	 * Processes UI updates from the inventory key being pressed 
	 */ 
	void OnInventoryKeyPressed();

	/**
	 * Processes UI updates from the escape key being pressed
	 */
	void OnEscapeKeyPressed();
	
	/**
	 * Triggers UI updates from the UseInventoryItem key being pressed
	 */
	void OnUseInventoryItemKeyPressed();

	/**
	 * Triggers UI updates from the DropInventoryItem key being pressed
	 */
	void OnDropInventoryItemKeyPressed();

	/**
	 * Gets the key binding for a specific action.
	 * 
	 * @param ActionName The name of the action as defined in Input settings in the editor
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FInputActionKeyMapping> GetKeyForAction(FName ActionName);

protected:
	/**
	 * Server side function for setting the player's focused interactable
	 *
	 * @param Actor The interactable actor to set as the focus
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetFocusedInteractable(AActor* Actor);

	/**
	 * Sets if the pawn is allowed to control the camera, used to disable camera input while UI menus are open and the cursor is active
	 *
	 * @param bCanLook Can the pawn control the camera?
	 */
	void SetPawnCanLook(bool bCanLook);

private:
	/**
	 * Performs line and sphere traces to find the closest interactable in range for the player to focus
	 */
	void CheckFocus();
	
};
