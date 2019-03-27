// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
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
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	 * Sets whether or not this actor can be interacted with. Only runs on the server.
	 * @param CanInteract Whether the actor should be interactable.
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerSetCanInteract(bool CanInteract);

	bool GetCanInteract();

	FText GetInteractionPromptText();

	void SetInteractionPromptText(FText NewInteractionPromptText);

	FText GetInteractableName();

	void SetInteractableName(FText NewInteractableName);

protected:
	/**
	 * Sets the mesh stencil value based on interactable type.
	 */
	void SetMeshStencilValue();
		
};
