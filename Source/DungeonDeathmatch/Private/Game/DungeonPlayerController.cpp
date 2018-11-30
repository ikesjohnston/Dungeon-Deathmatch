// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonPlayerController.h"
#include <CollisionQueryParams.h>
#include <UnrealNames.h>
#include <GameFramework/Actor.h>
#include <DrawDebugHelpers.h>
#include "Interactable.h"

// Console command for drawing interaction cast debug shapes
static int32 DebugInteraction = 0;
FAutoConsoleVariableRef CVARDebugInteraction(
	TEXT("Dungeon.DebugInteraction"),
	DebugInteraction,
	TEXT("Draw debug shapes for interaction casts"),
	ECVF_Cheat);

ADungeonPlayerController::ADungeonPlayerController()
{
	InteractionCastLenth = 400.0f;
	InteractionSweepRadius = 100.0f;
	PlayerForwardInteractionDistance = 10.0f;
}

void ADungeonPlayerController::Tick(float DeltaTime)
{
	CheckFocus();
}

void ADungeonPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonPlayerController, FocusedInteractable);
}

AInteractable* ADungeonPlayerController::GetFocusedInteractable()
{
	return FocusedInteractable;
}

void ADungeonPlayerController::Server_SetFocusedInteractable_Implementation(AInteractable* Interactable)
{
	FocusedInteractable = Interactable;
}

bool ADungeonPlayerController::Server_SetFocusedInteractable_Validate(AInteractable* Actor)
{
	return true;
}

void ADungeonPlayerController::CheckFocus()
{
	if (IsLocalController())
	{
		// Only run interaction checks when this controller is possessing a pawn
		APawn* PlayerPawn = GetPawn();
		if (!PlayerPawn)
		{
			if (FocusedInteractable)
			{
				FocusedInteractable->OnUnfocused();
				FocusedInteractable = nullptr;
				Server_SetFocusedInteractable(nullptr);
			}

			return;
		}

		// Just do a single sphere trace in front of the player and find the closest object
		AInteractable* ClosestInteractable = nullptr;
		TArray<FHitResult> SphereOutHits;

		// We don't want to detect interactables behind the player
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FVector PlayerForwadVector = PlayerPawn->GetActorForwardVector();
		FVector PlayerForwardLocation = PlayerLocation + (PlayerForwadVector * PlayerForwardInteractionDistance);
		FVector SphereSweepStart = PlayerLocation + (PlayerForwadVector * InteractionSweepRadius);
		FVector SphereSweepEnd = SphereSweepStart;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionSweepRadius);

		bool DidSphereSweepHit = GetWorld()->SweepMultiByChannel(SphereOutHits, SphereSweepStart, SphereSweepEnd, FQuat::Identity, TRACE_INTERACTABLE, Sphere);
		if (DebugInteraction)
		{
			DrawDebugSphere(GetWorld(), SphereSweepStart, InteractionSweepRadius, 32, FColor::Magenta);
			DrawDebugLine(GetWorld(), PlayerLocation, PlayerForwardLocation, FColor::Purple);
		}

		// If anything was hit by the sphere sweep, process those interactables
		if (DidSphereSweepHit)
		{
			// Find hit interactable closest to cursor
			for (FHitResult SweepHit : SphereOutHits)
			{
				AInteractable* HitInteractable = Cast<AInteractable>(SweepHit.Actor);
				if (HitInteractable)
				{
					if (ClosestInteractable)
					{
						float HitDistance = FVector::Distance(PlayerForwardLocation, HitInteractable->GetActorLocation());
						float ClosestDistance = FVector::Distance(PlayerForwardLocation, ClosestInteractable->GetActorLocation());
						if (HitDistance < ClosestDistance)
						{
							ClosestInteractable = HitInteractable;
						}
					}
					else
					{
						ClosestInteractable = HitInteractable;
					}
				}
			}
		}

		// If interactable was found, set as focus and render outline
		if (ClosestInteractable)
		{
			if (FocusedInteractable)
			{
				if (FocusedInteractable == ClosestInteractable)
				{
					return;
				}
				FocusedInteractable->OnUnfocused();
				if (DebugInteraction)
				{
					UE_LOG(LogTemp, Log, TEXT("ADungeonPlayerController::CheckFocus - Stopped focusing %s"), *FocusedInteractable->GetName());
				}
			}
			Server_SetFocusedInteractable(ClosestInteractable);
			FocusedInteractable = ClosestInteractable;
			FocusedInteractable->OnFocused();
			if (DebugInteraction)
			{
				UE_LOG(LogTemp, Log, TEXT("ADungeonPlayerController::CheckFocus - Started focusing %s"), *FocusedInteractable->GetName());
			}
		}
		else
		{
			if (FocusedInteractable)
			{
				FocusedInteractable->OnUnfocused();
				if (DebugInteraction)
				{
					UE_LOG(LogTemp, Log, TEXT("ADungeonPlayerController::CheckFocus - Stopped focusing %s"), *FocusedInteractable->GetName());
				}
				Server_SetFocusedInteractable(nullptr);
				FocusedInteractable = nullptr;
			}
		}
		
	}
}
