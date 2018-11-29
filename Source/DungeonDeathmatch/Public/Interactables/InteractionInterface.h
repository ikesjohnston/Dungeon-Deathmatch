// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "InteractionInterface.generated.h"

class ADungeonCharacter;

/**
 * Interface class that all interactable objects, such as doors, chests, and items, should implement. 
 */
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInteractionInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter);

	virtual void NativeOnEnterInteractionVolume();

	virtual void NativeOnExitInteractionVolume();

	virtual void NativeOnFocused();

	virtual void NativeOnUnfocused();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteract(ADungeonCharacter* InteractingCharacter);
};
