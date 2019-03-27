// Fill out your copyright notice in the Description page of Project Settings.

#include "RenderCaptureComponent.h"
#include "CharacterRenderCapture2D.h"

#include <GameFramework/Pawn.h>

URenderCaptureComponent::URenderCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("URenderCaptureComponent::URenderCaptureComponent - OwningCharacter is null."))
		return;
	}
}

void URenderCaptureComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the render capture actor, only do this for the local player character
	if (RenderCaptureClass && OwningCharacter->IsLocallyControlled())
	{
		RenderCaptureActor = Cast<ACharacterRenderCapture2D>(GetWorld()->SpawnActor(RenderCaptureClass));
		if (RenderCaptureActor)
		{
			RenderCaptureActor->InitializeRender(OwningCharacter);
			RenderCaptureActor->SetActorLocation(FVector(0, 0, -1000));
		}
	}
}

ACharacterRenderCapture2D* URenderCaptureComponent::GetRenderCaptureActor()
{
	return RenderCaptureActor;
}

void URenderCaptureComponent::UpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* Mesh)
{
	if (OwningCharacter->IsLocallyControlled() && RenderCaptureActor)
	{
		RenderCaptureActor->UpdateMeshSegment(MeshSegment, Mesh);
	}
}

void URenderCaptureComponent::AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativeLocation, FRotator RelativeRotation)
{
	if (OwningCharacter->IsLocallyControlled() && RenderCaptureActor)
	{
		RenderCaptureActor->AttachActorToSocket(Actor, SocketName, RelativeLocation, RelativeRotation);
	}
}

void URenderCaptureComponent::DetachActor(AActor* Actor)
{
	if (OwningCharacter->IsLocallyControlled() && RenderCaptureActor)
	{
		RenderCaptureActor->DetachActor(Actor);
	}
}
