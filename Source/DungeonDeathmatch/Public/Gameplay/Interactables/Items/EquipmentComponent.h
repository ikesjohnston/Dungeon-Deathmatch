// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentEnums.h"
#include "EquipmentComponent.generated.h"

class ADungeonCharacter;
class AEquippable;
class AArmor;
class AWeapon;

DECLARE_DELEGATE_OneParam(FOnItemEquipped, AEquippable*);
DECLARE_DELEGATE_OneParam(FOnItemUnequipped, AEquippable*);

/** Struct that stores the details of an individual weapon loadout */
USTRUCT(BlueprintType)
struct FWeaponLoadout
{
	GENERATED_BODY()

	/** The main hand weapon slot in the loadout. This weapon will determine if an off hand weapon can be used. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* MainHandWeapon;

	/** The off hand weapon loadout. This can only be used if MainHandWeapon is a OneHand weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* OffHandWeapon;

	/** The location on the character where this loadout is stored */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ELoadoutSheatheLocation SheatheLocation;

	FWeaponLoadout()
	{

	}
};

/** Actor component that stores equipped armor and weapon details */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/** The DungeonCharacter that this equipment component belongs to. Initialized when the game starts. */
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	ADungeonCharacter* OwningCharacter;

	/** The map of armor the player has equipped in each slot */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EArmorSlot, AArmor*> EquippedArmor;

	/** The first of two weapon loadouts available to the player */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|\Weapons")
	FWeaponLoadout PrimaryWeaponLoadout;

	/** The second of two weapon loadouts available to the player */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|\Weapons")
	FWeaponLoadout SecondaryWeaponLoadout;

private:

	FOnItemEquipped OnItemEquipped;

	FOnItemUnequipped OnItemUnequipped;

	/** Flag to determine if the primary loadout is active, is toggled on loadout switch */
	bool bIsPrimaryLoadoutActive;

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	//void Server_EquipItem(AEquippable* Equippable);

	//UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	//void Multicast_OnEquipItem(AEquippable* Equippable);

	//UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	//void Server_UnequipItem(AEquippable* Equippable);

	//UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	//void Multicast_OnUnequipItem(AEquippable* Equippable);

	//UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	//void Server_UnequipAll();

public:	
	
	/**
	 * Gets the map of armor pieces to each armor slot
	 *
	 * @return The armor mapping
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TMap<EArmorSlot, AArmor*> GetArmor() const;

	/**
	 * Gets the armor item in a particular armor slot
	 *
	 * @param Slot The armor slot to check
	 *
	 * @return The armor item in the slot, if any
	 */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	AArmor* GetArmorInSlot(EArmorSlot Slot);

	/**
	 * Gets the character's active loadout
	 *
	 * @return The active loadout
	 */
	FWeaponLoadout& GetActiveLoadout();

	/**
	 * Gets the character's inactive loadout
	 *
	 * @return The inactive loadout
	 */
	FWeaponLoadout& GetInactiveLoadout();

	/**
	 * Attempts to equip an item. May unequip or drop items in the process. This function will only run on the server.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param CanReplaceEquipment Whether an item existing in a valid equipment slot can be removed to make room for the new item
	 *
	 * @return Whether the item was equipped successfully
	 */
	bool TryEquipItem(AEquippable* Equippable, bool CanReplaceEquipment = false);

	/**
	 * Attempts to add a weapon to any open loadout slot. May unequip or drop items in the process. This function will only run on the server.
	 *
	 * @param Weapon The weapon to attempt to add to the loadout
	 * @param CanReplaceEquipment Whether a weapon existing in a valid loadout slot can be removed to make room for the new weapon
	 *
	 * @return Whether the weapon was added to a loadout successfully
	 */
	bool TryEquipWeapon(AWeapon* Weapon, bool CanReplaceEquipment = false);

	/**
	 * Attempts to add a weapon to a specific loadout slot. May unequip or drop a weapon in the process. This function will only run on the server.
	 *
	 * @param Weapon The weapon to attempt to add to the loadout
	 * @param Loadout The loadout to add the weapon to
	 * @param RequestedHand The hand to add the weapons to
	 * @param CanReplaceEquipment Whether an item existing in a valid equipment slot can be removed to make room for the new item
	 *
	 * @return Whether the weapon was added to the loadout slot successfully
	 */
	bool TryAddWeaponToLoadout(AWeapon* Weapon, FWeaponLoadout& Loadout, ERequestedHand RequestedHand);

	/**
	 * Unequips an item and attempts to add it back into the character's inventory. If there is no room in the inventory, the item will be dropped;.
	 *
	 * @param Equippable The item to attempt to equip
	 */
	void UnequipItem(AEquippable* Equippable);

private:
	/**
	 * Attempts to add a weapon to a specific loadout. This function will only run on the server.
	 *
	 * @param Weapon The weapon to add to the loadout
	 * @param Loadout The loadout to add the weapon to
	 * @param AddToOffHand Whether to add the weapon to the off hand slot instead of main hand, false by default
	 *
	 * @return Whether the weapon was added to the loadout slot successfully
	 */
	bool TryAddWeaponToLoadout(AWeapon* Weapon, FWeaponLoadout& Loadout, bool AddToOffHand = false);

	/**
	 * Removes weapon(s) from a specific loadout and places it back in the character's inventory or drops it. This function will only run on the server.
	 *
	 * @param Loadout The loadout to remove the weapon(s) from
	 * @param RequestedHand The hand(s) to remove the weapons from
	 */
	void RemoveWeaponFromLoadout(FWeaponLoadout& Loadout, ERequestedHand RequestedHand);
};
