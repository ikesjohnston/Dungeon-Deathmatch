// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Interactable.generated.h"

class ADungeonCharacter;
class UStaticMeshComponent;

/**
 * Base class for all interactables in the game, such as doors, chests, and items.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AInteractable : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** The prompt text that should appear on screen when this interactable is focused */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionPromptText;

public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetMeshComponent();

	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter) override;

	virtual void NativeOnEnterInteractionVolume() override;

	virtual void NativeOnExitInteractionVolume() override;

	virtual void NativeOnFocused() override;

	virtual void NativeOnUnfocused() override;
};
