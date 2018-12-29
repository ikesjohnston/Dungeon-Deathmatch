// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include <Components/MeshComponent.h>

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	bReplicates = true;
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionComponent, bCanInteract);
}

bool UInteractionComponent::GetCanInteract()
{
	return bCanInteract;
}

FText UInteractionComponent::GetInteractionPromptText()
{
	return InteractionPromptText;
}

void UInteractionComponent::SetInteractionPromptText(FText NewInteractionPromptText)
{
	InteractionPromptText = NewInteractionPromptText;
}

FText UInteractionComponent::GetInteractableName()
{
	return InteractableName;
}

void UInteractionComponent::SetInteractableName(FText NewInteractableName)
{
	InteractableName = NewInteractableName;
}

void UInteractionComponent::SetMeshStencilValue()
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

void UInteractionComponent::Server_SetCanInteract_Implementation(bool CanInteract)
{
	bCanInteract = CanInteract;
}

bool UInteractionComponent::Server_SetCanInteract_Validate(bool CanInteract)
{
	return true;
}

