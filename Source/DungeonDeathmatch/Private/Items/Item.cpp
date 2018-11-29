// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include <Components/MeshComponent.h>
#include <WidgetComponent.h>
#include "InventoryComponent.h"
#include "DungeonCharacter.h"
#include "EquipmentComponent.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weight = 1.0f;
	Value = 1.0f;
}

AItem::~AItem()
{

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FText AItem::GetItemName()
{
	return ItemName;
}

float AItem::GetWeight()
{
	return Weight;
}

float AItem::GetValue()
{
	return Value;
}

UTexture2D* AItem::GetIcon()
{
	return Icon;
}

void AItem::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	Super::NativeOnInteract(InteractingCharacter);

	/*UInventoryComponent* CharacterInventory = Cast<UInventoryComponent>(InteractingCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
	if(CharacterInventory)
	{
		CharacterInventory->Server_AddItem(this);
	}*/
}
