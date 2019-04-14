// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "EquipmentGlobals.h"
#include "EquipmentComponent.generated.h"

class AEquippable;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquipped, AEquippable*, Equippable, EEquipmentSlot, EquipmentSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnequipped, AEquippable*, Equippable, EEquipmentSlot, EquipmentSlot);

/** Actor component that stores equipped armor and weapons. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UInventoryComponent;

	/* Delegate called when item is equipped; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemEquipped OnItemEquipped;

	/* Delegate called when item is unequipped; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnItemUnequipped OnItemUnequipped;

protected:
	/** The mapping of equipment the actor has equipped in each slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, AEquippable*> Equipment;

	/** Is the first loadout active, or is it the second? Determines combat animations and weapon placement. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bIsPrimaryLoadoutActive;

	/** Mapping of socket types to socket names, used for attaching weapons to the character mesh during equipment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Sockets")
	TMap<EWeaponSocketType, FName> WeaponSocketMap;

public:	
	UEquipmentComponent();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TMap<EEquipmentSlot, AEquippable*> GetEquipment() const { return Equipment; };

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AEquippable* GetEquipmentInSlot(EEquipmentSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<EEquipmentSlot> GetValidSlotsForEquippable(AEquippable* Equippable);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<EEquipmentSlot> GetOpenSlotsForEquippable(AEquippable* Equippable);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipItem(AEquippable* Equippable, bool TryMoveReplacementToInventory = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipItemToSlot(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUnequipItem(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDetachActor(AActor* Actor);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggleActiveLoadout();

	bool IsPrimaryLoadoutActive() { return bIsPrimaryLoadoutActive; };

	UFUNCTION(BlueprintPure, Category = "Equipment")
	FWeaponLoadout GetActiveWeaponLoadout();

	/** Gets socket name on the owning actor's mesh for a given weapon socket type */
	FName GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType);

protected:
	TArray<EEquipmentSlot> GetOpenSlots(TArray<EEquipmentSlot> Slots);

	/** Attempts to equip an item to the specified slot.  This function will only run on the server. */
	bool RequestEquipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/** Attempts to unequip an item from a specific slot. This function will only run on the server. */
	bool RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot Slot);

	/** Broadcasts OnItemEquipped event for UI updates. Should only be called from server side functions. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnItemEquipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/** Broadcasts OnItemUnequipped event for UI updates. Should only be called by the server. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnItemUnequipped(AEquippable* Equippable, EEquipmentSlot EquipmentSlot);

	/** Attaches an actor to the character mesh at the specified socket. Should only be called by the server. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/** Detaches an actor from the character mesh. Should only be called by the server. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDetachActor(AActor* Actor);

	AWeapon* GetOtherHandWeaponToUnequip(AWeapon* Weapon, EEquipmentSlot EquipmentSlot);

};
