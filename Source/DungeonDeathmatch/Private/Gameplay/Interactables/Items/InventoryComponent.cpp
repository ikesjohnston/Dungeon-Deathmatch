// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "Item.h"
#include "DungeonCharacter.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	bReplicates = true;

	InventoryCapactiy = 16;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, Gold);
}

TArray<AItem*> UInventoryComponent::GetInventory()
{
	return Inventory;
}

uint8 UInventoryComponent::GetInventoryCapacity()
{
	return InventoryCapactiy;
}

bool UInventoryComponent::TryAddItem(AItem* Item)
{
	bool Result = false;
	if (Item && Inventory.Num() < InventoryCapactiy)
	{
		Server_AddItem(Item);
		Result = true;
	}

	return Result;
}

bool UInventoryComponent::TryRemoveItem(AItem* Item)
{
	bool Result = false;
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == Item)
		{
			Server_RemoveItem(i);
			Result = true;
		}
	}
	return Result;
}

void UInventoryComponent::Server_AddItem_Implementation(AItem* Item)
{
	Multicast_OnItemAdd(Item);
	Inventory.Add(Item);
}

bool UInventoryComponent::Server_AddItem_Validate(AItem* Item)
{
	return true;
}

void UInventoryComponent::Server_RemoveItem_Implementation(int32 InventoryIndex)
{
	AItem* ItemToRemove = Inventory[InventoryIndex];
	Multicast_OnItemRemove(ItemToRemove);
	Inventory.RemoveAtSwap(InventoryIndex);
}

bool UInventoryComponent::Server_RemoveItem_Validate(int32 InventoryIndex)
{
	return true;
}

void UInventoryComponent::Multicast_OnItemAdd_Implementation(AItem* Item)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemAdded.Broadcast(Item);
	}
	// Hide item in world and move it to origin
	Item->SetActorEnableCollision(false);
	Item->GetMeshComponent()->SetSimulatePhysics(false);
	Item->GetMeshComponent()->SetVisibility(false);
	Item->SetActorLocation(FVector(0, 0, 0));
}

void UInventoryComponent::Multicast_OnItemRemove_Implementation(AItem* Item)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnItemRemoved.Broadcast(Item);
	}
	// Set the item back to visible and drop it at the owner's location
	Item->SetActorEnableCollision(true);
	Item->GetMeshComponent()->SetSimulatePhysics(true);
	Item->GetMeshComponent()->SetVisibility(true);
	Item->SetActorLocation(GetOwner()->GetActorLocation());
}
