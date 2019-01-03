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
 * Controller class for the player, currently only used to do ray tracing on the client.
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

	/* The distance from the camera to multi sphere trace for interactables. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionCameraTraceDistance;

	/* The radius of the multi sphere trace from the camera for interactables. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionCameraTraceRadius;

	/* The distance in front of the player to trace for interactables. A sphere trace will be cast with a radius equal to half of this distance if the initial multi sphere trace fails. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionPlayerTraceDistance;

	/* The InventoryEquipmentSlotWidget that the player is currently mousing over. */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UInventoryEquipmentSlotWidget* HoveringInventoryEquipmentSlot;

	/* Separate debug variable to be used in blueprint for drawing debug traces; set by the CVAR Dungeon.DebugInteraction */
	UPROPERTY(BlueprintReadOnly)
	bool bIsDebuggingInteraction;

private:
	float InteractionPlayerTraceRadius;

public:
	ADungeonPlayerController();

	virtual void Tick(float DeltaTime) override;

	AActor* GetFocusedInteractable();

	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryEquipmentSlotWidget* GetHoveringInventoryEquipmentSlot();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHoveringInventoryEquipmientSlot(UInventoryEquipmentSlotWidget* InventoryEquipmentSlot);

	void OnInventoryKeyPressed();

	void OnEscapeKeyPressed();

	void OnUseInventoryItemKeyPressed();

	void OnDropInventoryItemKeyPressed();

	/**
	 * Get key bindings for specific action.
	 * @param ActionName The name of the action as defined in Input settings in the editor
	 */
	UFUNCTION(BlueprintCallable)
	TArray<FInputActionKeyMapping> GetKeyForAction(FName ActionName);

protected:
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetFocusedInteractable(AActor* Actor);

	void SetPawnCanLook(bool bCanLook);

private:
	void CheckFocus();
	
};
