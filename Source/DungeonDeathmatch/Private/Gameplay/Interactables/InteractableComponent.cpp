// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComponent.h"

#include <Components/MeshComponent.h>

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	bReplicates = true;
}

// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, bCanInteract);
}

bool UInteractableComponent::GetCanInteract()
{
	return bCanInteract;
}

FText UInteractableComponent::GetInteractionPromptText()
{
	return InteractionPromptText;
}

void UInteractableComponent::SetInteractionPromptText(FText NewInteractionPromptText)
{
	InteractionPromptText = NewInteractionPromptText;
}

FText UInteractableComponent::GetInteractableName()
{
	return InteractableName;
}

void UInteractableComponent::SetInteractableName(FText NewInteractableName)
{
	InteractableName = NewInteractableName;
}

void UInteractableComponent::SetMeshStencilValue()
{
	
	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Render custom depth on all mesh components. Certain interactables may have multiple mesh components.
		TArray<UActorComponent*> MeshComponents = Owner->GetComponentsByClass(UMeshComponent::StaticClass());
		for (int i = 0; i < MeshComponents.Num(); i++)
		{
			UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
			if (MeshComp)
			{
				MeshComp->SetCustomDepthStencilValue(QualityTierStencilValue);
			}
		}
	}
	
}

void UInteractableComponent::ServerSetCanInteract_Implementation(bool CanInteract)
{
	bCanInteract = CanInteract;
}

bool UInteractableComponent::ServerSetCanInteract_Validate(bool CanInteract)
{
	return true;
}

