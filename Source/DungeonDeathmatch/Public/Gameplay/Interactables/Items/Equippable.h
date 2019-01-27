// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "EquipmentGlobals.h"
#include "Equippable.generated.h"

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	ADungeonCharacter* EquippingCharacter;

public:
	// Sets default values for this actor's properties
	AEquippable(const FObjectInitializer& ObjectInitializer);

	virtual ~AEquippable();

	virtual FText GetInventoryUseTooltipText() override;

	/** Server side function for calling OnEquip events. */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnEquip(ADungeonCharacter* NewEquippingCharacter);

	/** Server side function for calling OnUnequip events. */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnUnequip();

protected:
	/** Multicast function for calling OnEquip event on all clients. Should only be called by server side functions. */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEquip(ADungeonCharacter* NewEquippingCharacter);

	/** Multicast function for calling OnUnequip event on all clients. Should only be called by server side functions. */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnUnequip();

	/** Override event that will only be called on the server when the item is equipped. Should only be called directly by Server_OnEquip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void ServerOnEquip(ADungeonCharacter* NewEquippingCharacter);

	/** Override event that will only be called on the server when the item is unequipped. Should only be called directly by Server_OnUnequip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void ServerOnUnequip();

	/** Override event that will be called on all clients when the item is equipped. Should only be called directly by Multicast_OnEquip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void MulticastOnEquip(ADungeonCharacter* NewEquippingCharacter);

	/** Override event that will be called on all clients when the item is unequipped. Should only be called directly by Multicast_OnUnequip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void MulticastOnUnequip();
};
