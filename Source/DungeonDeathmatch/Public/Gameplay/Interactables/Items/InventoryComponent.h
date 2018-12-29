// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/* Event delegate for when an item is added to the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, class AItem*, Item);
/* Event delegate for when an item is removed from the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemovedSignature, class AItem*, Item);

class AItem;

/**
 * Component class for storing Item Actors on another actor.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	/* Delegate called when item is added. For UI updates. */
	FOnItemAddedSignature OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	/* Delegate called when item is removed. For UI updates. */
	FOnItemRemovedSignature OnItemRemoved;

protected:
	/* Internal data representation of the items in the inventory */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AItem*> Inventory;

	/* The number of items that can be held in this inventory. Used for UI elements and to determine if new items can be stored. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	uint8 InventoryCapactiy;

	/* The amount of gold in this inventory. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0))
	int32 Gold;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** 
	 * Adds an item to the inventory. Only called on the server.
	 *
	 * @param Item The item to be added to the inventory
	 */
	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_AddItem(AItem* Item);

	/** 
	 * Performs physics and rendering updates for an added item. Called on server and all clients.
	 *
	 * @param Item The item that was added to the inventory
	 */
	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemAdd(AItem* Item);

	/** 
	 * Removes an item from the inventory. Only called on the server.
	 *
	 * @param InventoryIndex The index of the item to be removed
	 */
	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_RemoveItem(int32 InventoryIndex);

	/**
	 * Performs physics and rendering updates for a dropped item. Called on server and all clients.
	 *
	 * @param Item The item that was removed from the inventory
	 */ 
	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemRemove(AItem* Item);


public:	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<AItem*> GetInventory();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	uint8 GetInventoryCapacity();

	/**
	 * Local function that checks if it is valid to add an item to the inventory, and makes a server call to do so if valid.
	 *
	 * @param Item The item to be added to the inventory
	 */
	bool TryAddItem(AItem* Item);

	/**
	 * Local function that checks if it is valid to remove an item from the inventory, and makes a server call to do so if valid.
	 *
	 * @param Item The item to be added to the inventory
	 */
	bool TryRemoveItem(AItem* Item);
};
