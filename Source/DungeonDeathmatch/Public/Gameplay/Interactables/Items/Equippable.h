// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interactables/Items/Item.h"
#include "Equippable.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	MainHand					UMETA(DisplayName = "Main Hand"),
	OffHand						UMETA(DisplayName = "Off Hand"),
	Head						UMETA(DisplayName = "Head"),
	Chest						UMETA(DisplayName = "Chest"),
	Legs						UMETA(DisplayName = "Legs"),
	Gloves						UMETA(DisplayName = "Gloves"),
	Boots						UMETA(DisplayName = "Boots"),
	Amulet						UMETA(DisplayName = "Amulet"),
	RingOne						UMETA(DisplayName = "RingOne"),
	RingTwo						UMETA(DisplayName = "RingTwo"),
	Consumable					UMETA(DisplayName = "Consumable"),

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
	/** What equipment slot(s) can this item be equipped in? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<EEquipmentSlot> ValidEquipmentSlots;

	/** What equipment slot(s) will this item lock while equipped? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<EEquipmentSlot> LockedEquipmentSlots;

public:
	// Sets default values for this actor's properties
	AEquippable();
	virtual ~AEquippable();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnEquip(ADungeonCharacter* EquippingCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnUnequip(ADungeonCharacter* UnequippingCharacter);
};
