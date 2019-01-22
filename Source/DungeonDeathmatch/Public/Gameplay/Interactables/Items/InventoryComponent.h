// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"

/* Event delegate for when an item is added to the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedSignature, class AItem*, Item, FInventoryGridPair, OriginGridSlot);

/* Event delegate for when an item is removed from the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedSignature, class AItem*, Item, FInventoryGridPair, OriginGridSlot);

/* Event delegate for when the size of the inventory changes */
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

	/** The number of slots to the right of this slot before reaching an occupied slot, updated whenever an item is added or removed from the grid. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 NumberOfOpenSlotsRight;

	/** The number of slots below this slot before reaching an occupied slot, updated whenever an item is added or removed from the grid. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 NumberOfOpenSlotsBelow;

	FInventoryGridSlot()
	{
		Item = nullptr;
		StartingGridLocation = FInventoryGridPair();
		NumberOfOpenSlotsRight = 0;
		NumberOfOpenSlotsBelow = 0;
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
	/* Delegate called when item is added. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAddedSignature OnItemAdded;

	/* Delegate called when item is removed. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemovedSignature OnItemRemoved;

	/* Delegate called when the inventory size changes. For UI updates. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventorySizeChangedSignature OnInventorySizeChanged;

protected:
	/* The list of items in the inventory */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AItem*> InventoryItems;

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

	/**
	 * Initializes the grid array, called at game start or whenever the inventory size changes
	 */
	void InitializeGrid();

	/**
	 * Adds an item to the inventory at the specified grid location. Only called on the server.
	 *
	 * @param Item The item to be added to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_AddItem(AItem* Item, FInventoryGridPair OriginSlot);

	/** 
	 * Broadcasts OnItemAdded event for UI updates
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
	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_RemoveItem(int32 InventoryIndex);

	/**
	 * Broadcasts OnItemRemoved event for UI updates
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

public:	
	/**
	 * Returns the list of inventory items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<AItem*> GetInventory();

	/**
	 * Returns the size of the inventory grid
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventoryGridPair GetInventoryGridSize();

	/**
	 * Local function that checks if it is valid to add an item to the inventory, and makes a server call to do so if valid.
	 *
	 * @param Item The item to be added to the inventory
	 */
	bool TryAddItem(AItem* Item);

	/**
	 * Local function that checks if it is valid to add an item to the inventory at the specified grid location, and makes a server call to do so if valid.
	 *
	 * @param Item The item to be added to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	bool TryAddItem(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Local function that checks if it is valid to remove an item from the inventory, and makes a server call to do so if valid.
	 *
	 * @param Item The item to be added to the inventory
	 */
	bool TryRemoveItem(AItem* Item);

	/**
	 * Local function that makes a server call to change the size of the inventory grid and then broadcasts and event for UI updates.
	 *
	 * @param Rows The new amount of rows for the inventory grid
	 * @param Columns The new amount of columns for the inventory grid
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ChangeInventorySize(uint8 Rows, uint8 Columns);
};
