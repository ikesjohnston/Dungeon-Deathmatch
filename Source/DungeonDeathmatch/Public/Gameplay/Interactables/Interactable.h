// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class ADungeonCharacter;
class UStaticMeshComponent;
class UWidgetComponent;

/**
 * Interface class that all interactable objects, such as doors, chests, items, and NPCs should implement.
 */
UINTERFACE(BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInteractable
{
	GENERATED_IINTERFACE_BODY()

public:
	/**
	 * Event to be fired when a player interacts with this interactable. Must be implemented by classes that implement this interface, but can be 
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(ADungeonCharacter* InteractingCharacter);

	/**
	 * Event to be fired when a player focuses this interactable. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnFocused();

	/**
	 * Event to be fired when a player stops focusing this interactable. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnUnfocused();

	/**
	 * Gets whether or not the interactable can currently be interacted with. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool GetCanInteract();

	/**
	 * Sets whether or not this actor can be interacted with.
	 * @param CanInteract Whether the actor should be interactable. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetCanInteract(bool CanInteract);

	/**
	 * Gets the text to be displayed for interaction tooltips. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPromptText();

	/**
	 * Gets the name of the interactable to be displayed for interaction tooltips. Must be implemented by classes that implement this interface, but can be
	 * overridden in blueprint as well.
	 */ 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractableName();
};
