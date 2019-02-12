// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentGlobals.h"
#include "EquipmentComponent.generated.h"

class ADungeonCharacter;
class AEquippable;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquipped, AEquippable*, Equippable, EEquipmentSlot, EquipmentSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnequipped, AEquippable*, Equippable, EEquipmentSlot, EquipmentSlot);

/** Actor component that stores equipped armor and weapon details */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Delegate called when item is equipped; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemEquipped OnItemEquipped;

	/* Delegate called when item is unequipped; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemUnequipped OnItemUnequipped;

protected:

	/** The DungeonCharacter that this equipment component belongs to. Initialized when the game starts. */
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	ADungeonCharacter* OwningCharacter;

	/** The map of equipment the player has equipped in each slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, AEquippable*> Equipment;

	/** Is the first loadout active, or is it the second? Determines combat animations and weapon placement. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|\Weapons")
	bool bIsPrimaryLoadoutActive;

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	/** Gets the map of equipment the player has equipped in each slot */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TMap<EEquipmentSlot, AEquippable*> GetEquipment() const;

	/**
	 * Gets the equipped item in a particular equipment slot
	 *
	 * @param Slot The equipment slot to check
	 *
	 * @return The equipment in the slot, if any
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	AEquippable* GetEquipmentInSlot(EEquipmentSlot Slot);

	/**
	 * Gets the valid slots that an equippable can be equipped to
	 *
	 * @param Equippable The equippable to check valid slots for
	 *
	 * @return The list of valid slots
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<EEquipmentSlot> GetValidSlotsForEquippable(AEquippable* Equippable);

	/**
	 * Gets the open slots, if any, that an equippable can be equipped to
	 *
	 * @param Equippable The equippable to check open slots for
	 *
	 * @return The list of open slots, if any
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<EEquipmentSlot> GetOpenSlotsForEquippable(AEquippable* Equippable);

	/**
	 * Attempts to equip an item to the specified slot.  This function will only run on the server.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param Slot The equipment slot this item will go into
	 *
	 * @return Whether the item was equipped successfully
	 */
	bool RequestEquipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/**
	 * Attempts to unequip an item from a specific slot. This function will only run on the server.
	 *
	 * @param Equippable The item to attempt to unequip
	 * @param Slot The equipment slot this item will be removed from
	 *
	 * @return Whether the item was unequipped successfully
	 */
	bool RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/** Server side function that toggles which loadout is the current active loadout */
	UFUNCTION(Server, Reliable, WithValidation, Category = "Equipment")
	void Server_ToggleActiveLoadout();

	/** Is the primary loadout active, or is the second? */
	bool IsPrimaryLoadoutActive();

	/** Gets the active weapon loadout, which contains pointers to the weapons in each hand */
	UFUNCTION(BlueprintCallable)
	FWeaponLoadout GetActiveWeaponLoadout();

protected:
	/**
	 * Equips an item to the specified slot.  This function will only run on the server.
	 *
	 * @param Equippable The item to equip
	 * @param Slot The equipment slot this item will go into
	 */
	void EquipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/**
	 * Unequips an item from a specific slot. This function will only run on the server.
	 *
	 * @param Equippable The item to unequip
	 * @param Slot The equipment slot this item will be removed from
	 */
	void UnequipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/**
	 * Broadcasts OnItemEquipped event for UI updates. Should only be called from server side functions.
	 *
	 * @param Equippable The item that was equipped
	 * @param EquipmentSlot The equipment slot the item went into
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void Multicast_OnItemEquipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/**
	 * Broadcasts OnItemUnequipped event for UI updates. Should only be called from server side functions.
	 *
	 * @param Equippable The item that was unequipped
	 * @param EquipmentSlot The equipment slot the item was removed from
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void Multicast_OnItemUnequipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/** Toggles which loadout is the current active loadout */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void Multicast_ToggleActiveLoadout();
};
