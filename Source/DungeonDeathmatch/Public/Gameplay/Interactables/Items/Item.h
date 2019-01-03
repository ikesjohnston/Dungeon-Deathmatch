// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Item.generated.h"

class ADungeonCharacter;

/**
 * Enum representation of the different quality levels for items. Higher quality items are generally more rare and valuable.
 * Used for UI and post process elements.
 */
UENUM(BlueprintType) enum class EItemQualityTier : uint8 {
	Normal		UMETA(DisplayName = "Normal"),
	Uncommon	UMETA(DisplayName = "Uncommon"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary")
};

/**
 * Base class for all items in the game.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AItem : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:
	/* The visual representation of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/* Widget used to display tooltips on interact focus. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* WidgetComponent;

	/* Flag for toggling ability to interact. Replicated to all clients. */
	UPROPERTY(Replicated, EditAnywhere, Category = "Interaction")
	bool bCanInteract;

	/* The prompt text that should appear on screen when this item is focused by the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionPromptText;

	/* The stencil value to use when rendering the post process outline for this item, based on its quality */
	uint8 QualityTierStencilValue;

	/* The name of this item. Used by UI classes. */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText ItemName;

	/* How much gold is this item worth? */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	float Value;

	/* Optional descriptive text about the item */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText FlavorText;

	/* Icon texture for this item to represent it in the UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;

	/* The quality of this item. Higher quality items are generally more rare and valuable.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemQualityTier QualityTier;

	/* The color corresponding to the quality of the item that is used for UI and post process elements */
	FLinearColor QualityTierColor;

	/* The text to display for the use prompt on the item tooltip. Ex. "Use", "Equip", etc. */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText InventoryUseTooltipText;

public:	
	// Sets default values for this actor's properties
	AItem(const FObjectInitializer& ObjectInitializer);

	virtual ~AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * Sets the mesh stencil value based on item type and quality.
	 */
	void SetMeshStencilValue();

	/**
	 * Native implementation of interaction event. Can be overridden by subclasses.
	 * Should only be called internally by the item during a server side interaction event.
	 *
	 * @param InteractingCharacter The character that is interacting with this item
	 */
	//virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Gets the mesh component for the item
	 */
	UStaticMeshComponent* GetMeshComponent();

	/**
	 * Gets the name of the item
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemName();

	/**
	 * Gets the value of the item
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	float GetValue();

	/**
	 * Gets the optional descriptive text for the item, if any
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetFlavorText();

	/**
	 * Gets the icon texture for the item
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	UTexture2D* GetIcon();

	/**
	 * Gets the quality color of the item
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	FLinearColor GetQualityTierColor();

	/**
	 * Gets the tooltip text for the item to use for interaction prompts
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	virtual FText GetInventoryUseTooltipText();

	/**
	 * Attempts to use the item if it is currently in the player's inventory
	 */
	UFUNCTION()
	bool TryInventoryUse();

public:

	// ------------------------ BEGIN INTERACTABLE INTERFACE FUNCTIONS ------------------------
	virtual void OnInteract_Implementation(ADungeonCharacter* InteractingCharacter) override;


	virtual void OnFocused_Implementation() override;


	virtual void OnUnfocused_Implementation() override;


	virtual bool GetCanInteract_Implementation() override;


	virtual void SetCanInteract_Implementation(bool CanInteract) override;


	virtual FText GetInteractionPromptText_Implementation() override;


	virtual FText GetInteractableName_Implementation() override;
	// ------------------------ END INTERACTABLE INTERFACE FUNCTIONS ------------------------
};
