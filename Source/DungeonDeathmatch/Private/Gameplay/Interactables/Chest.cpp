// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include <WidgetComponent.h>
#include "InteractionComponent.h"
#include "Loot/LootComponent.h"
#include "ChestAnimInstance.h"


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

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->Server_SetCanInteract(true);
	InteractionComponent->SetInteractionPromptText(FText::FromString("Open"));
	InteractionComponent->SetInteractableName(FText::FromString("Chest"));

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

//void AChest::OnInteract(ADungeonCharacter* InteractingCharacter)
//{
//	if (Role == ROLE_Authority)
//	{
//		// Just set open flag to true, the animation blueprint will then call loot events through AnimNotifies
//		bIsOpened = true;
//		// Need to call OnRep functions manually for server in C++
//		OnRep_IsOpened();
//
//		// Should no longer be able to interact with chest
//		SetCanInteract(false);
//	}
//}
//
//void AChest::OnFocused()
//{
//	// Add glowing outline to mesh(es). Set by the post processing object in the level. This should only happen on the client.
//	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
//	for (int i = 0; i < MeshComponents.Num(); i++)
//	{
//		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
//		if (MeshComp)
//		{
//			MeshComp->SetRenderCustomDepth(true);
//		}
//	}
//
//	WidgetComponent->SetVisibility(true);
//}
//
//void AChest::OnUnfocused()
//{
//	// Remove glowing outline from mesh(es). Set by the post processing object in the level. This should only happen on the client.
//	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
//	for (int i = 0; i < MeshComponents.Num(); i++)
//	{
//		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
//		if (MeshComp)
//		{
//			MeshComp->SetRenderCustomDepth(false);
//		}
//	}
//
//	WidgetComponent->SetVisibility(false);
//}
//
//UWidgetComponent* AChest::GetWidgetComponent()
//{
//	return WidgetComponent;
//}
//
//bool AChest::GetCanInteract()
//{
//	return InteractionComponent->GetCanInteract();
//}
//
//void AChest::SetCanInteract(bool CanInteract)
//{
//	InteractionComponent->Server_SetCanInteract(CanInteract);
//}
//
//FText AChest::GetInteractionPromptText()
//{
//	return InteractionComponent->GetInteractionPromptText();
//}
//
//FText AChest::GetInteractableName()
//{
//	return InteractionComponent->GetInteractableName();
//}

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