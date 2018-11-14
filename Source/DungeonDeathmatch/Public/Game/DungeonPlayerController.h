// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ADungeonPlayerController();

	virtual void Tick(float DeltaTime) override;

	AActor* GetCurrentFocus();

protected:

	/** The Actor that is currently focused by the player */
	UPROPERTY(Replicated)
	AActor* CurrentFocus;

	/** The most recent hit result of raytrace from the player's camera */
	FHitResult PlayerFocusHitResult;

protected:

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetCurrentFocus(AActor* Actor);

private:

	void CheckCurrentFocus();
	
};
