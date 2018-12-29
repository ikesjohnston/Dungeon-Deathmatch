// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "DungeonCharacter.h"
#include <Components/StaticMeshComponent.h>
#include "DungeonDeathmatch.h"
#include <Engine/EngineTypes.h>
#include <WidgetComponent.h>
#include "InteractTooltipWidget.h"

UInteractable::UInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);
	//MeshComponent->SetCollisionResponseToChannel(TRACE_INTERACTABLE, ECR_Ignore);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetVisibility(false);

	QualityTierStencilValue = STENCIL_ITEM_DEFAULT;

	bCanInteract = true;
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetMeshStencilValue();

	// Initialize interaction tooltip
	UInteractTooltipWidget* InteractTooltip = Cast<UInteractTooltipWidget>(WidgetComponent->GetUserWidgetObject());
	if (InteractTooltip)
	{
		InteractTooltip->SetInteractable(this);
	}
}

void AInteractableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableActor, bCanInteract);
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* AInteractableActor::GetMeshComponent()
{
	return MeshComponent;
}

void AInteractableActor::SetMeshStencilValue()
{
	// Render custom depth on all mesh components. Certain interactables (like chests or special weapons) may have multiple mesh components.
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCustomDepthStencilValue(QualityTierStencilValue);
		}
	}
}

bool AInteractableActor::GetCanInteract()
{
	return bCanInteract;
}

void AInteractableActor::Server_SetCanInteract(bool CanInteract)
{
	bCanInteract = CanInteract;
}

FText AInteractableActor::GetInteractionPromptText()
{
	return InteractionPromptText;
}

FText AInteractableActor::GetInteractableName()
{
	return InteractableName;
}

void AInteractableActor::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	// Server side safety check
	if (Role == ROLE_Authority)
	{

	}
}

void AInteractableActor::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	// Server side safety check
	if (Role == ROLE_Authority)
	{
		NativeOnInteract(InteractingCharacter);
	}
}

void AInteractableActor::Server_OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	OnInteract(InteractingCharacter);
}

bool AInteractableActor::Server_OnInteract_Validate(ADungeonCharacter* InteractingCharacter)
{
	return true;
}

void AInteractableActor::OnFocused()
{
	// Add glowing outline to mesh(es). Set by the post processing object in the level. This should only happen on the client.
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(true);
		}
	}

	WidgetComponent->SetVisibility(true);
}

void AInteractableActor::OnUnfocused()
{
	// Remove glowing outline from mesh(es). Set by the post processing object in the level. This should only happen on the client.
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(false);
		}
	}

	WidgetComponent->SetVisibility(false);
}

void IInteractable::OnFocused()
{

}

void IInteractable::OnUnfocused()
{

}

UWidgetComponent* IInteractable::GetWidgetComponent()
{
	return nullptr;
}

bool IInteractable::GetCanInteract()
{
	return false;
}

void IInteractable::SetCanInteract(bool CanInteract)
{

}

FText IInteractable::GetInteractionPromptText()
{
	return FText();
}

FText IInteractable::GetInteractableName()
{
	return FText();
}
