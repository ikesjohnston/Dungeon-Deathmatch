// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryGlobals.h"
#include "InventoryComponent.generated.h"

class AItem;

/* Event delegate for when an item is added to the inventory; for UI updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedSignature, AItem*, Item, FInventoryGridPair, OriginGridSlot);
/* Event delegate for when an item is removed from the inventory; for UI updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedSignature, AItem*, Item, FInventoryGridPair, OriginGridSlot);
/* Event delegate for when the size of the inventory changes; for UI updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySizeChangedSignature, uint8, GridRows, uint8, GridColumns);

/** Struct that stores inventory grid slot information */
USTRUCT(BlueprintType)
struct FInventoryGridSlot
{
	GENERATED_BODY()

	/** The item that is currently occupying this grid slot. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AItem* Item;

	/** The coordinates of the origin slot for the item occupying this slot. Used to efficiently void out slots if the item is moved or removed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FInventoryGridPair StartingGridLocation;

	FInventoryGridSlot()
	{
		Item = nullptr;
		StartingGridLocation = FInventoryGridPair();
	}
};

/**
 * Component class for storing Item Actors on another actor.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Delegate called when item is added; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAddedSignature OnItemAdded;

	/* Delegate called when item is removed; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemovedSignature OnItemRemoved;

	/* Delegate called when the inventory size changes; for UI updates */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventorySizeChangedSignature OnInventorySizeChanged;

protected:
	/* The list of items stored in the inventory */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AItem*> Items;

	/* The grid representation of items in the inventory and their placement */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryGridSlot> InventoryGrid;

	/* The number of rows and columns that make up the inventory grid. */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FInventoryGridPair InventoryGridSize;

	/** The force to apply to items when dropping them */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropEjectionForce;

	/** The relative location to drop items */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (MakeEditWidget = true))
	FVector ItemDropRelativeLocation;

public:	
	UInventoryComponent();
	
	TArray<AItem*> GetItems();

	FInventoryGridPair GetInventoryGridSize();

	FVector GetItemDropLocation();

	/**
	 * Server side function that attempts to add an item to the character's inventory and makes a multicast RPC with the result. Used for items that are currently "despawned" and may only be visible from UI elements.
	 *
	 * @param Item The item to attempt to add to the inventory
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	virtual void ServerRequestAddItemToInventory(AItem* Item);

	/**
	* Attempts to add an item to the inventory and returns the result. Only runs on the server.
	*
	* @param Item The desired item to be added to the inventory
	*/
	bool RequestAddItem(AItem* Item);

	/**
	 * Server side function that attempts to add an item to the character's inventory at the specified location and makes a multicast RPC with the result. Used for items that are currently "despawned" and may only be visible from UI elements.
	 *
	 * @param Item The item to attempt to add to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	virtual void ServerRequestAddItemToInventoryAtLocation(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Attempts to add an item to the inventory at the specified grid location and returns the result. Only runs on the server.
	 *
	 * @param Item The desired item to be added to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	bool RequestAddItem(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Server side function that attempts to pick up an item and add it to the character's inventory and makes a multicast RPC with the result. Used when interacting with items in the world.
	 *
	 * @param Item The item to attempt to pick up
	 */
	UFUNCTION(Server, Unreliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestPickUpItem(AItem* Item);

	/**
	 * Server side function that attempts to remove an item from the character's inventory and makes a multicast RPC with the result. Used for items that should remain "despawned" and only be visible from UI elements.
	 *
	 * @param Item The item to attempt to remove from the inventory
	 */
	UFUNCTION(Server, Unreliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestRemoveItemFromInventory(AItem* Item);

	/**
	 * Attempts to remove an item from the inventory and returns the result. Only runs on the server.
	 *
	 * @param Item The desired item to be removed from the inventory
	 */
	bool RequestRemoveItem(AItem* Item);

	/**
	 * Server side function that attempts to remove an item from the character's inventory and "spawn" it in front of the character and makes a multicast RPC with the result.
	 *
	 * @param Item The item to attempt to drop
	 * @param CheckInventory Whether to check if the item is in the inventory before dropping it. This should only not be done when dropping via Drag & Drop operation from the inventory
	 * in which case the item has already been removed from the inventory.
	 */
	UFUNCTION(Server, Unreliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestDropItem(AItem* Item, bool CheckInventory = true);

protected:
	virtual void BeginPlay() override;

	/**
	 * Adds an item to the inventory at the specified grid location. Only called on the server.
	 *
	 * @param Item The item to be added to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	void AddItem(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Broadcasts OnItemAdded event for UI updates. Should only be called from server side functions.
	 *
	 * @param Item The item that was added to the inventory
	 * @param OriginSlot The upper left most slot that this item occupies in the inventory grid
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void MulticastOnItemAdded(AItem* Item, FInventoryGridPair OriginSlot);

	/** 
	 * Removes an item from the inventory. Only called on the server.
	 *
	 * @param InventoryIndex The index of the item to be removed
	 */
	void RemoveItem(int32 InventoryIndex);

	/**
	 * Broadcasts OnItemRemoved event for UI updates. Should only be called from server side functions.
	 *
	 * @param Item The item that was removed from the inventory
	 * @param OriginSlot The upper left most slot that this item occupies in the inventory grid
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void MulticastOnItemRemoved(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Local function that makes a server call to change the size of the inventory grid and then broadcasts an event for UI updates.
	 *
	 * @param Rows The new amount of rows for the inventory grid
	 * @param Columns The new amount of columns for the inventory grid
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ChangeInventorySize(uint8 Rows, uint8 Columns);

	/**
	 * Changes the size of the inventory grid. Only called on the server.
	 *
	 * @param Rows The new amount of rows for the inventory grid
	 * @param Columns The new amount of columns for the inventory grid
	 */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerChangeInventorySize(uint8 Rows, uint8 Columns);
};
