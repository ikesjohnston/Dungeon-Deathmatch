// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/PlayerController.h"
#include <GameFramework/PlayerInput.h>
#include "DungeonCursorWidget.h"
#include "DungeonPlayerController.generated.h"

class UInteractable;
class AItem;
class UDungeonCursorWidget;
class UDraggableItemWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusedSignature, class UInteractable*, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableUnfocusedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginItemDragSignature, class AItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndItemDragSignature, class AItem*, Item);

/**
 * Controller class for the player, handles much of the UI and interaction logic.
 * Exists on the server and owning client.
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/* Delegate called when interactable is focused by player. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableFocusedSignature OnInteractableFocused;

	/* Delegate called when interactable is unfocused by player. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableUnfocusedSignature OnInteractableUnfocused;

	/* Delegate called when an item drag operation starts. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnBeginItemDragSignature OnBeginItemDrag;

	/* Delegate called when an item drag operation end. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnEndItemDragSignature OnEndItemDrag;

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

	/* The cursor widget being controlled by this player */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UDungeonCursorWidget* Cursor;

	/* The DraggableItemWidget associated with the current drag and drop operation, if any */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UDraggableItemWidget* DraggedItem;

	/* The DraggableItemWidget currently being moused over, if any */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UDraggableItemWidget* SelectedItem;

	bool bCanTraceForInteractables;

public:
	ADungeonPlayerController();

	virtual void Tick(float DeltaTime) override;

	/**
	 * Get the interactable actor that the player currently has focused, if any
	 */
	AActor* GetFocusedInteractable();

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

	/**
	 * Sets the input focus to the UI and Game, used to refresh input focus in the event that an individual widget takes the focus
	 */
	UFUNCTION(BlueprintCallable)
	void FocusUIAndGame();

	/**
	 * Sets the input focus to the Game only, used to relinquish mouse cursor input
	 */
	void FocusGame();

	/** Gets the mouse cursor widget being controlled by this player */
	UFUNCTION(BlueprintCallable)
	UDungeonCursorWidget* GetCursor();

	/** Sets the mouse cursor widget being controlled by this player */
	UFUNCTION(BlueprintCallable)
	void SetCursor(UDungeonCursorWidget* NewCursor);

	/** Gets the DraggableItemWidget associated with the current drag and drop operation, if any */
	UFUNCTION(BlueprintPure)
	UDraggableItemWidget* GetDraggedItem();

	/** Sets the DraggableItemWidget currently being moused over */
	UFUNCTION(BlueprintCallable)
	void SetSelectedItem(UDraggableItemWidget* DraggableItemWidget);

	/** Gets the DraggableItemWidget currently being moused over */
	UFUNCTION(BlueprintPure)
	UDraggableItemWidget* GetSelectedItem();

	/** Sets the DraggableItemWidget associated with the current drag and drop operation. Updates the mouse cursor with the image of any item being dragged */
	UFUNCTION(BlueprintCallable)
	void StartDraggingItem(UDraggableItemWidget* DraggableItemWidget);

	/** Stops the current drag and drop operation. */
	UFUNCTION(BlueprintCallable)
	void StopDraggingItem(bool WasCanceled);

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
