// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

class AInteractable;

/**
 * Controller class for the player, currently only used to do ray tracing on the client.
 * Exists on the server and owning client.
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()
	

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

private:
	void CheckFocus();
	
};
