// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

class AInteractable;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocusedSignature, class AInteractable*, Interactable);
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
	/** The Interactable that is currently focused by the player */
	UPROPERTY(Replicated)
	AInteractable* FocusedInteractable;

	/** The distance to ray cast forward from the camera for interactables */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCastLenth;

	/** The radius of the multi sphere sweep for interactables */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionSweepRadius;

	/** The distance in front of the player to check interactable distance from */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float PlayerForwardInteractionDistance;

public:
	ADungeonPlayerController();

	virtual void Tick(float DeltaTime) override;

	AInteractable* GetFocusedInteractable();

	void OnInventoryKeyPressed();

	void OnEscapeKeyPressed();

protected:
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetFocusedInteractable(AInteractable* Actor);

	void SetPawnCanLook(bool bCanLook);

private:
	void CheckFocus();
	
};
