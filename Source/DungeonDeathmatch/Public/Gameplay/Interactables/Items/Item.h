// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Item.generated.h"

class ADungeonCharacter;

UENUM(BlueprintType) enum class EItemQualityTier : uint8 {
	Normal		UMETA(DisplayName = "Normal"),
	Uncommon	UMETA(DisplayName = "Uncommon"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary")
};

UCLASS()
class DUNGEONDEATHMATCH_API AItem : public AInteractableActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText ItemName;

	/* How much gold is this item worth? */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	float Value;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText FlavorText;

	/* Icon texture for this item to represent it in the UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemQualityTier QualityTier;

	FLinearColor QualityTierColor;

	/* The text to display for the use prompt on the item tooltip. Ex. "Use", "Equip", etc. */
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText InventoryUseTooltipText;

public:	
	// Sets default values for this actor's properties
	AItem();

	virtual ~AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Sets the post process stencil value for this item based on its quality */
	void SetQualityTierStencilValue();

	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemName();

	UFUNCTION(BlueprintPure, Category = "Item")
	float GetValue();

	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetFlavorText();

	UFUNCTION(BlueprintPure, Category = "Item")
	UTexture2D* GetIcon();

	UFUNCTION(BlueprintPure, Category = "Item")
	FLinearColor GetQualityTierColor();

	UFUNCTION(BlueprintPure, Category = "Item")
	virtual FText GetInventoryUseTooltipText();

	UFUNCTION()
	bool TryInventoryUse();
};
