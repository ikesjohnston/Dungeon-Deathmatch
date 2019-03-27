// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Item.h"
#include "EquipmentGlobals.h"
#include "Equippable.generated.h"

UCLASS()
class DUNGEONDEATHMATCH_API AEquippable : public AItem
{
	GENERATED_BODY()
	
	friend class UEquipmentComponent;

protected:
	/* A reference to the object currently equipping this item, if any. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AActor* EquippingActor;

public:
	AEquippable(const FObjectInitializer& ObjectInitializer);

	virtual ~AEquippable();

	virtual FText GetInventoryUseTooltipText() override;

	/** Server side function for calling OnEquip events. */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnEquip(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot);

	/** Server side function for calling OnUnequip events. */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnUnequip();

protected:
	/** Multicast function for calling OnEquip event on all clients. Should only be called by server side functions. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnEquip(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot);

	/** Multicast function for calling OnUnequip event on all clients. Should only be called by server side functions. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnUnequip();

	/** Override event that will only be called on the server when the item is equipped. Should only be called directly by ServerOnEquip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void ServerOnEquipEvent(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot);

	/** Override event that will only be called on the server when the item is unequipped. Should only be called directly by ServerOnUnequip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void ServerOnUnequipEvent();

	/** Override event that will be called on all clients when the item is equipped. Should only be called directly by MulticastOnEquip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void MulticastOnEquipEvent(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot);

	/** Override event that will be called on all clients when the item is unequipped. Should only be called directly by MulticastOnUnequip. */
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void MulticastOnUnequipEvent();
};
