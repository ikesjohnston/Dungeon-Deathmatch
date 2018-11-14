// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	TArray<AItem*> Inventory;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<AItem*>& GetInventory();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_AddItem(AItem* Item);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void Server_RemoveItem(AItem* Item);

	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemAdd(AItem* Item);

	UFUNCTION(NetMulticast, Unreliable, Category = "Inventory")
	void Multicast_OnItemRemove(AItem* Item);
};
