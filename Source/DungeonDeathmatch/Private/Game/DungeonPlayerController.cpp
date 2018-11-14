// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonPlayerController.h"
#include "Functionable.h"
#include <CollisionQueryParams.h>
#include <UnrealNames.h>
#include <GameFramework/Actor.h>
#include <DrawDebugHelpers.h>




ADungeonPlayerController::ADungeonPlayerController()
{

}

void ADungeonPlayerController::Tick(float DeltaTime)
{
	CheckCurrentFocus();
}

void ADungeonPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonPlayerController, CurrentFocus);
}

AActor* ADungeonPlayerController::GetCurrentFocus()
{
	return CurrentFocus;
}

void ADungeonPlayerController::Server_SetCurrentFocus_Implementation(AActor* Actor)
{
	CurrentFocus = Actor;
}

bool ADungeonPlayerController::Server_SetCurrentFocus_Validate(AActor* Actor)
{
	return true;
}

void ADungeonPlayerController::CheckCurrentFocus()
{
	if (IsLocalController())
	{
		APawn* MyPawn = GetPawn();

		if (!MyPawn)
		{
			IFunctionable* Functionable = Cast<IFunctionable>(CurrentFocus);
			if (Functionable)
			{
				Functionable->NativeOnFocusEnd();

				// Focused Actor won't replicate?
				CurrentFocus = NULL;
				Server_SetCurrentFocus(NULL);
			}

			return;
		}

		// Determine start and end points of ray trace
		FVector TraceStart;
		FRotator CameraRot;

		GetPlayerViewPoint(TraceStart, CameraRot);

		TraceStart = PlayerCameraManager->GetCameraLocation();

		const FVector TraceDirection = CameraRot.Vector();
		const FVector TraceEnd = TraceStart + (TraceDirection * 600);

		// Set up ray trace
		FCollisionQueryParams TraceParams(NAME_None, true, MyPawn);
		TraceParams.AddIgnoredActor(MyPawn);
		TraceParams.bTraceAsyncScene = true;

		// Perform ray trace
		PlayerFocusHitResult.Reset();
		GetWorld()->LineTraceSingleByChannel(PlayerFocusHitResult, TraceStart, TraceEnd, TRACE_PLAYER_ACTIVATION, TraceParams);

		// Draw Debug line
		/*if (PlayerFocusHitResult.GetActor())
		{
			DrawDebugLine(GetWorld(), TraceStart, PlayerFocusHitResult.TraceEnd, FColor::Green, true, 1.0f, 10.f);
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, true, 1.0f, 10.f);
		}*/

		AActor* NewFocus = PlayerFocusHitResult.GetActor();

		if (CurrentFocus != NewFocus)
		{
			IFunctionable* Functionable = Cast<IFunctionable>(CurrentFocus);
			if (Functionable)
			{
				Functionable->NativeOnFocusEnd();
			}

			// Focused Actor won't replicate?
			CurrentFocus = NewFocus;
			Server_SetCurrentFocus(NewFocus);

			Functionable = Cast<IFunctionable>(CurrentFocus);
			if (Functionable)
			{
				Functionable->NativeOnFocusStart();
			}
		}

		/*if (PlayerFocusHitResult.GetActor())
		{
			ActivationPoint = PlayerFocusHitResult.ImpactPoint;
		}*/
	}
}
