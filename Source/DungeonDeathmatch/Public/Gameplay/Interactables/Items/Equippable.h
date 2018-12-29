// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interactables/Items/Item.h"
#include "Equippable.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	MainHand		UMETA(DisplayName = "Main Hand"),
	OffHand			UMETA(DisplayName = "Off Hand"),
	Head			UMETA(DisplayName = "Head"),
	Chest			UMETA(DisplayName = "Chest"),
	Legs			UMETA(DisplayName = "Legs"),
	Hands			UMETA(DisplayName = "Hands"),
	Neck			UMETA(DisplayName = "Neck"),
	Finger			UMETA(DisplayName = "Finger"),

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
	AEquippable();

	virtual ~AEquippable();

	/* Returns an array of EEquipmentSlots that it is valid to place this item into when equipping it. */
	virtual TArray<EEquipmentSlot> GetValidEquipmentSlots();

	/* Returns an array of EEquipmentSlots that are locked and can't have items equipped in them while this item is equipped. */
	virtual TArray<EEquipmentSlot> GetLockedEquipmentSlots();

	virtual FText GetInventoryUseTooltipText() override;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnEquip(ADungeonCharacter* NewEquippingCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnUnequip();
};
