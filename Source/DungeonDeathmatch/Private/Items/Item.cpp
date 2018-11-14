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

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	bReplicates = true;
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

void AItem::NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	UInventoryComponent* CharacterInventory = Cast<UInventoryComponent>(FunctioningCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
	if(CharacterInventory)
	{
		CharacterInventory->Server_AddItem(this);
	}
}

void AItem::NativeOnFocusStart()
{
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(true);
		}
	}
}

void AItem::NativeOnFocusEnd()
{
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(false);
		}
	}
}

UStaticMeshComponent* AItem::GetMeshComponent()
{
	return Mesh;
}

void AItem::Server_OnPickup(ADungeonCharacter* Character)
{
	// Testing code, to be removed
	UEquipmentComponent* CharacterEquipment = Cast<UEquipmentComponent>(Character->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (CharacterEquipment)
	{
		CharacterEquipment->Server_EquipItem(this);
	}
}

void AItem::Server_OnEquip(ADungeonCharacter* Character)
{

}
