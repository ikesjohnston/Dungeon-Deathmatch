// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include "InteractableComponent.h"
#include "LootComponent.h"
#include "ChestAnimInstance.h"

#include <WidgetComponent.h>

AChest::AChest()
{
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetVisibility(false);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractionComponent"));
	InteractableComponent->ServerSetCanInteract(true);
	InteractableComponent->SetInteractionPromptText(FText::FromString("Open"));
	InteractableComponent->SetInteractableName(FText::FromString("Chest"));

	LootComponent = CreateDefaultSubobject<ULootComponent>(TEXT("LootComponent"));

	bIsOpened = false;
}

void AChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChest, bIsOpened);
}

bool AChest::GetIsOpened()
{
	return bIsOpened;
}

void AChest::OnRep_IsOpened()
{
	UChestAnimInstance* ChestAnimInstance = Cast<UChestAnimInstance>(MeshComponent->GetAnimInstance());
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