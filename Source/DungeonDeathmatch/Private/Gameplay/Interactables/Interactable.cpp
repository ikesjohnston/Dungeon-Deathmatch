// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "DungeonCharacter.h"
#include <Components/StaticMeshComponent.h>
#include "DungeonDeathmatch.h"
#include <Engine/EngineTypes.h>
#include <WidgetComponent.h>
#include "InteractTooltipWidget.h"

// Sets default values
AInteractable::AInteractable()
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
void AInteractable::BeginPlay()
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

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractable, bCanInteract);
}

void AInteractable::SetMeshStencilValue()
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

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* AInteractable::GetMeshComponent()
{
	return MeshComponent;
}

bool AInteractable::GetCanInteract()
{
	return bCanInteract;
}

void AInteractable::SetCanInteract(bool CanInteract)
{
	bCanInteract = CanInteract;
}

FText AInteractable::GetInteractionPromptText()
{
	return InteractionPromptText;
}

FText AInteractable::GetInteractableName()
{
	return InteractableName;
}

void AInteractable::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{

}

void AInteractable::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	NativeOnInteract(InteractingCharacter);
}

void AInteractable::OnFocused()
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

void AInteractable::OnUnfocused()
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
