// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryGlobals.h"
#include "InventoryComponent.generated.h"

class AItem;

/** Struct that stores inventory grid slot information */
USTRUCT(BlueprintType)
struct FInventoryGridSlot
{
	GENERATED_BODY()

	/** The item that is currently occupying this grid slot. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AItem* Item;

	/** The coordinates of the origin slot for the item occupying this slot. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FInventoryGridPair ItemOriginGridLocation;

	FInventoryGridSlot()
	{
		Item = nullptr;
		ItemOriginGridLocation = FInventoryGridPair();
	}
};

/* Event delegate for when an item is added to the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedSignature, AItem*, Item, FInventoryGridPair, OriginGridSlot);
/* Event delegate for when an item is removed from the inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedSignature, AItem*, Item, FInventoryGridPair, OriginGridSlot);

/** Actor component that stores inventory items. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class UEquipmentComponent;

	/* Delegate called when an item is added (for UI updates) */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAddedSignature OnItemAdded;

	/* Delegate called when an item is removed (for UI updates) */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemovedSignature OnItemRemoved;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (MakeEditWidget = true))
	FVector ItemDropRelativeLocation;

public:	
	UInventoryComponent();
	
	TArray<AItem*> GetItems() { return Items; };

	TArray<FInventoryGridSlot> GetInventoryGrid() { return InventoryGrid; };

	FInventoryGridPair GetInventoryGridSize() { return InventoryGridSize; };

	FVector GetItemDropLocation();

	/** Server side function that attempts to add an item to the actor's inventory. Used for items that are currently "despawned" and may only be visible from UI elements. */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerRequestAddItemToInventory(AItem* Item);

	/** Server side function that attempts to add an item to the actor's inventory at the specified location. Used for items that are currently "despawned" and may only be visible from UI elements. */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerRequestAddItemToInventoryAtLocation(AItem* Item, FInventoryGridPair OriginSlot);

	/** Server side function that attempts to pick up an item and add it to the actor's inventory. Used when interacting with items in the world. */
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestPickUpItem(AItem* Item);

	/** Server side function that attempts to remove an item from the actor's inventory. Used for items that should remain "despawned" and only be visible from UI elements. */
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestRemoveItemFromInventory(AItem* Item);

	/** Server side function that attempts to remove an item from the actor's inventory and "spawn" it in front of the actor. */
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory")
	virtual void ServerRequestDropItem(AItem* Item, bool CheckInventory = true);

protected:
	virtual void BeginPlay() override;

	/** Attempts to add an item to the inventory and returns the result. Only runs on the server. */
	bool RequestAddItem(AItem* Item);

	/** Attempts to add an item to the inventory at the specified grid location and returns the result. Only runs on the server. */
	bool RequestAddItem(AItem* Item, FInventoryGridPair OriginSlot);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void MulticastOnItemAdded(AItem* Item, FInventoryGridPair OriginSlot);

	/** Attempts to remove an item from the inventory and returns the result. Only runs on the server. */
	bool RequestRemoveItem(AItem* Item);

	UFUNCTION(NetMulticast, Reliable, Category = "Inventory")
	void MulticastOnItemRemoved(AItem* Item, FInventoryGridPair OriginSlot);

private:
	void AddItem(AItem* Item, FInventoryGridPair &OriginSlot);

	void RemoveItem(int32 ItemIndex);

	bool ValidateItem(AItem* Item);

};
