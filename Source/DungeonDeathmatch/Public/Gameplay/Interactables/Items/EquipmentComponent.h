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
	friend class IEquipmentInterface;

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bIsPrimaryLoadoutActive;

	/** Mapping of sock types to socket names, used for attaching weapons to the character mesh during equipment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Sockets")
	TMap<EWeaponSocketType, FName> WeaponSocketMap;

	/** The name of the socket corresponding to the first hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Sockets")
	FString SocketNameConsumableOne;

	/** The name of the socket corresponding to the second hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Sockets")
	FString SocketNameConsumableTwo;

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
	 * Server side function that attempts to equip an item to any valid equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param TryMoveReplacementToInventory Should an attempt be made to move any item already in the specified slot back to the character's inventory?
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerEquipItem(AEquippable* Equippable, bool TryMoveReplacementToInventory = false);

	/**
	 * Server side function that attempts to equip an item to a specific equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param EquipmentSlot The equipment slot to attempt to put the item into
	 * @param TryMoveReplacementToInventory Should an attempt be made to move any item already in the specified slot back to the character's inventory?
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerEquipItemToSlot(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory = false);

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
	 * Server side function that attempts to unequip an item from a specific equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to unequip
	 * @param EquipmentSlot The equipment slot to attempt to remove the item from
	 * @param TryMoveToInventory Should an attempt be made to move the unequipped item back to the character's inventory?
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerUnequipItem(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory = false);

	/**
	 * Attempts to unequip an item from a specific slot. This function will only run on the server.
	 *
	 * @param Equippable The item to attempt to unequip
	 * @param Slot The equipment slot this item will be removed from
	 *
	 * @return Whether the item was unequipped successfully
	 */
	bool RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/**
	 * Server call to attach an actor to the character mesh at the specified socket.
	 *
	 * @param Actor The actor to attach
	 * @param SocketName The name of the socket to attach the actor to
	 * @param RelativePosition The relative position adjustment to transform the actor by after attachment
	 * @param RelativeRotation The relative rotation adjustment to transform the actor by after attachment
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/**
	 * Server call to detach an actor from the character mesh.
	 *
	 * @param Actor The actor to detach
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDetachActor(AActor* Actor);

	/** Server side function that toggles which loadout is the current active loadout */
	UFUNCTION(Server, Reliable, WithValidation, Category = "Equipment")
	void ServerToggleActiveLoadout();

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
	 * Broadcasts OnItemEquipped event for UI updates. Should only be called from server side functions.
	 *
	 * @param Equippable The item that was equipped
	 * @param EquipmentSlot The equipment slot the item went into
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void MulticastOnItemEquipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/**
	 * Unequips an item from a specific slot. This function will only run on the server.
	 *
	 * @param Equippable The item to unequip
	 * @param Slot The equipment slot this item will be removed from
	 */
	void UnequipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/**
	 * Broadcasts OnItemUnequipped event for UI updates. Should only be called from server side functions.
	 *
	 * @param Equippable The item that was unequipped
	 * @param EquipmentSlot The equipment slot the item was removed from
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void MulticastOnItemUnequipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/** Gets socket name on the character for a given weapon socket type*/
	FName GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType);

	/**
	 * Attaches an actor to the character mesh at the specified socket. Should only be called by the server.
	 *
	 * @param Actor The actor to attach
	 * @param SocketName The name of the socket to attach the actor to
	 * @param RelativePosition The relative position adjustment to transform the actor by after attachment
	 * @param RelativeRotation The relative rotation adjustment to transform the actor by after attachment
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/**
	 * Detaches an actor from the character mesh. Should only be called by the server.
	 *
	 * @param Actor The actor to detach
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDetachActor(AActor* Actor);

	/** Toggles which loadout is the current active loadout */
	UFUNCTION(NetMulticast, Reliable, Category = "Equipment")
	void MulticastToggleActiveLoadout();

};
