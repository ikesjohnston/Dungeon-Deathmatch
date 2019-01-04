// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interactables/Items/Item.h"
#include "Equippable.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Head			UMETA(DisplayName = "Head"),
	Shoulders		UMETA(DisplayName = "Shoulders"),
	Chest			UMETA(DisplayName = "Chest"),
	Waist			UMETA(DisplayName = "Waist"),
	Legs			UMETA(DisplayName = "Legs"),
	Feet			UMETA(DisplayName = "Feet"),
	Hands			UMETA(DisplayName = "Hands"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Consumable		UMETA(DisplayName = "Consumable"),

	NUM_EQUIPMENT_SLOTS
};

/**
 * Base class for all equippable items in the game.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AEquippable : public AItem
{
	GENERATED_BODY()
	
	friend class UEquipmentComponent;

protected:
	/* A reference to the character currently equipping this item, if any. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	ADungeonCharacter* EquippingCharacter;

public:
	// Sets default values for this actor's properties
	AEquippable(const FObjectInitializer& ObjectInitializer);

	virtual ~AEquippable();

	/* Returns an array of EEquipmentSlots that it is valid to place this item into when equipping it. */
	virtual TArray<EEquipmentSlot> GetValidEquipmentSlots();

	/* Returns an array of EEquipmentSlots that are locked and can't have items equipped in them while this item is equipped. */
	virtual TArray<EEquipmentSlot> GetLockedEquipmentSlots();

	virtual FText GetInventoryUseTooltipText() override;
	
	// ------------------------ BEGIN INTERACTABLE INTERFACE OVERRIDES ------------------------
	virtual void OnInteract_Implementation(ADungeonCharacter* InteractingCharacter) override;
	// ------------------------ END INTERACTABLE INTERFACE OVERRIDES ------------------------

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnEquip(ADungeonCharacter* NewEquippingCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnUnequip();
};
