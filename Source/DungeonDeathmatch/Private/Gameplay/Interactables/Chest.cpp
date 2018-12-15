// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include "Loot/LootComponent.h"

AChest::AChest()
{
	ChestMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMeshComponent"));
	ChestMeshComponent->SetupAttachment(MeshComponent);
	ChestMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChestMeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);

	LootComponent = CreateDefaultSubobject<ULootComponent>(TEXT("LootComponent"));

	bIsOpened = false;
}

void AChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChest, bIsOpened);
}

void AChest::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	// Just set open flag to true, the animation blueprint will then call loot events through AnimNotifies
	bIsOpened = true;

	// Should no longer be able to interact with chest
	SetCanInteract(false);
}

void AChest::Loot()
{
	if (Role == ROLE_Authority)
	{
		LootComponent->GenerateLoot();
	}
}
