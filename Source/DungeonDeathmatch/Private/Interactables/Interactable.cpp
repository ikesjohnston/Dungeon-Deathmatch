// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "DungeonCharacter.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
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

void AInteractable::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{

}

void AInteractable::NativeOnEnterInteractionVolume()
{

}

void AInteractable::NativeOnExitInteractionVolume()
{

}

void AInteractable::NativeOnFocused()
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
}

void AInteractable::NativeOnUnfocused()
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
}
