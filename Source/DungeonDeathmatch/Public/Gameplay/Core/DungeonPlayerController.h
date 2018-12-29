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

	/* The distance to ray cast forward from the camera for interactables */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCastLenth;

	/* The radius of the multi sphere sweep for interactables */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionSweepRadius;

	/* The distance in front of the player to check interactable distance from */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float PlayerForwardInteractionDistance;

	/* The InventoryEquipmentSlotWidget that the player is currently mousing over. */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UInventoryEquipmentSlotWidget* HoveringInventoryEquipmentSlot;

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
