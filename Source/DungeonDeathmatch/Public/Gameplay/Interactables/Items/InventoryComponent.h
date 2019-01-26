// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryGlobals.h"
#include "InventoryComponent.generated.h"

/* Event delegate for when an item is added to the inventory; for UI updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedSignature, class AItem*, Item, FInventoryGridPair, OriginGridSlot);

/* Event delegate for when an item is removed from the inventory; for UI updates */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedSignature, class AItem*, Item, FInventoryGridPair, OriginGridSlot);

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

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	 * Returns the list of items stored in the inventory
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<AItem*> GetItems();

	/**
	 * Returns the size of the inventory grid
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventoryGridPair GetInventoryGridSize();

	/**
	 * Attempts to add an item to the inventory and returns the result. Only runs on the server.
	 *
	 * @param Item The desired item to be added to the inventory
	 */
	bool RequestAddItem(AItem* Item);

	/**
	 * Attempts to add an item to the inventory at the specified grid location and returns the result. Only runs on the server.
	 *
	 * @param Item The desired item to be added to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	bool RequestAddItem(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Attempts to remove an item from the inventory and returns the result. Only runs on the server.
	 *
	 * @param Item The desired item to be removed from the inventory
	 */
	bool RequestRemoveItem(AItem* Item);

	/**
	 * Local function that makes a server call to change the size of the inventory grid and then broadcasts an event for UI updates.
	 *
	 * @param Rows The new amount of rows for the inventory grid
	 * @param Columns The new amount of columns for the inventory grid
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ChangeInventorySize(uint8 Rows, uint8 Columns);

protected:
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
	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemAdd(AItem* Item, FInventoryGridPair OriginSlot);

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
	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemRemove(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Changes the size of the inventory grid. Only called on the server.
	 *
	 * @param Rows The new amount of rows for the inventory grid
	 * @param Columns The new amount of columns for the inventory grid
	 */
	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_ChangeInventorySize(uint8 Rows, uint8 Columns);
};
