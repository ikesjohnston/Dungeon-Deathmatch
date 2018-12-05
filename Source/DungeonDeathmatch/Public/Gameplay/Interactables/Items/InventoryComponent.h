// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, class AItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemovedSignature, class AItem*, Item);

class AItem;

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AItem*> Inventory;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	float InventoryCapactiy;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemAdd(AItem* Item);

	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemRemove(AItem* Item);

public:	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<AItem*> GetInventory();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_AddItem(AItem* Item);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_RemoveItem(AItem* Item);
};
