// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Interactable.generated.h"

class ADungeonCharacter;
class UStaticMeshComponent;
class UWidgetComponent;

/**
 * Base class for all interactables in the game, such as doors, chests, and items.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AInteractable : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/* Widget used to display tooltips on interact focus. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgetComponent;
	
	/* Flag for toggling ability to interact. Replicated to all clients. */
	UPROPERTY(Replicated, EditAnywhere, Category = "Interaction")
	bool bCanInteract;

	/* The prompt text that should appear on screen when this interactable is focused. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionPromptText;

	/* The name of this interactable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableName;

	/* The stencil value to use when rendering the post process outline for this interactable, based on its quality. */
	uint8 QualityTierStencilValue;

public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * Sets the mesh stencil value based on interactable type.
	 */ 
	void SetMeshStencilValue();

	/**
	 * Native implementation of interaction event. Can be overridden by subclasses. 
	 * Should only be called internally by the interactable during a server side interaction event.
	 * 
	 * @param InteractingCharacter The character that is interacting with this actor
	 */
	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter);

	/**
	 * Blueprint native event called when player interacts with this actor. Used to extend functionality to blueprint. 
	 * Should only be called internally by the interactable during a server side interaction event.
	 * 
	 * @param InteractingCharacter The character that is interacting with this actor
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(ADungeonCharacter* InteractingCharacter);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetMeshComponent();

	bool GetCanInteract();

	/* Sets whether or not this actor can be interacted with. Only runs on the server. */
	void Server_SetCanInteract(bool CanInteract);

	FText GetInteractionPromptText();

	FText GetInteractableName();

	/**
	 * Server side call for processing interaction events.
	 * 
	 * @param InteractingCharacter The character that is interacting with this actor
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation)
	void Server_OnInteract(ADungeonCharacter* InteractingCharacter);

	virtual void OnFocused();

	virtual void OnUnfocused();

};
