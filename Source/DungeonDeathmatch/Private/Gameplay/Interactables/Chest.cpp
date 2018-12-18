// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include "Loot/LootComponent.h"
#include "ChestAnimInstance.h"

AChest::AChest()
{
	bReplicates = true;

	ChestMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMeshComponent"));
	ChestMeshComponent->SetupAttachment(MeshComponent);
	ChestMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChestMeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);

	LootComponent = CreateDefaultSubobject<ULootComponent>(TEXT("LootComponent"));

	bIsOpened = false;
}

bool AChest::GetIsOpened()
{
	return bIsOpened;
}

void AChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChest, bIsOpened);
}

void AChest::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	if (Role == ROLE_Authority)
	{
		// Just set open flag to true, the animation blueprint will then call loot events through AnimNotifies
		bIsOpened = true;

		// Need to call OnRep functions manually for server in C++
		OnRep_IsOpened();

		// Should no longer be able to interact with chest
		Server_SetCanInteract(false);
	}
}

void AChest::OnRep_IsOpened()
{
	UChestAnimInstance* ChestAnimInstance = Cast<UChestAnimInstance>(ChestMeshComponent->GetAnimInstance());
	if (ChestAnimInstance)
	{
		ChestAnimInstance->SetIsOpen(bIsOpened);
	}
}

void AChest::Loot()
{
	if (Role == ROLE_Authority)
	{
		LootComponent->GenerateLoot();
	}
}
