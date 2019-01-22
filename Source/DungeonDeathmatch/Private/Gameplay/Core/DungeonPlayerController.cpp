// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonPlayerController.h"
#include <CollisionQueryParams.h>
#include <UnrealNames.h>
#include <GameFramework/Actor.h>
#include <DrawDebugHelpers.h>
#include "Interactable.h"
#include "DungeonHUD.h"
#include "DungeonCharacter.h"
#include <GameFramework/PlayerController.h>
#include "Item.h"
#include "DraggableItemWidget.h"

// Console command for drawing interaction cast debug shapes
static int32 DebugInteraction = 0;
FAutoConsoleVariableRef CVARDebugInteraction(
	TEXT("Dungeon.DebugInteraction"),
	DebugInteraction,
	TEXT("Draw debug shapes for interaction casts"),
	ECVF_Cheat);

ADungeonPlayerController::ADungeonPlayerController()
{
	InteractionCameraTraceForwardOffset = 10.0f;
	InteractionCameraTraceDistance = 1000.0f;
	InteractionCameraTraceRadius = 25.0f;

	MaxInteractionDistance = 200.0f;
}

void ADungeonPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFocus();
}

AActor* ADungeonPlayerController::GetFocusedInteractable()
{
	return FocusedInteractable;
}

void ADungeonPlayerController::OnInventoryKeyPressed()
{
	ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD)
	{
		if (DungeonHUD->IsCharacterMenuVisible())
		{
			DungeonHUD->HideCharacterMenu();
			DungeonHUD->HideInventoryMenu();
			DungeonHUD->ShowReticle();
			bShowMouseCursor = false;
			bEnableClickEvents = false;
			bEnableMouseOverEvents = false;
			SetPawnCanLook(true);
			StopDraggingItem();
			FocusGame();
		}
		else
		{
			DungeonHUD->ShowCharacterMenu();
			DungeonHUD->ShowInventoryMenu();
			DungeonHUD->HideReticle();
			bShowMouseCursor = true;
			bEnableClickEvents = true;
			bEnableMouseOverEvents = true;
			SetPawnCanLook(false);
			FocusUIAndGame();
		}
	}
}

void ADungeonPlayerController::OnEscapeKeyPressed()
{
	ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD)
	{
		DungeonHUD->HideCharacterMenu();
		bShowMouseCursor = false;
		SetPawnCanLook(false);	
		StopDraggingItem();
		FocusGame();
	}
}

void ADungeonPlayerController::OnUseInventoryItemKeyPressed()
{

}

void ADungeonPlayerController::OnDropInventoryItemKeyPressed()
{
	/*ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD && DungeonHUD->IsCharacterMenuVisible() && HoveringInventoryEquipmentSlot)
	{
		AItem* SlottedItem = HoveringInventoryEquipmentSlot->GetItem();
		ADungeonCharacter* DungeonPawn = Cast<ADungeonCharacter>(GetPawn());
		if (DungeonPawn)
		{
			DungeonPawn->Server_TryDropItem(SlottedItem);
		}
	}*/
}

TArray<FInputActionKeyMapping> ADungeonPlayerController::GetKeyForAction(FName ActionName)
{
	return PlayerInput->GetKeysForAction(ActionName);
}

void ADungeonPlayerController::FocusUIAndGame()
{
	ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD)
	{
		FInputModeGameAndUI InputMode = FInputModeGameAndUI();
		TSharedPtr<SWidget> WidgetPtr = DungeonHUD->GetInGameOverlay()->TakeWidget();
		InputMode.SetWidgetToFocus(WidgetPtr);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
}

void ADungeonPlayerController::FocusGame()
{
	FInputModeGameOnly InputMode = FInputModeGameOnly();
	SetInputMode(InputMode);
}

UDungeonCursorWidget* ADungeonPlayerController::GetCursor()
{
	return Cursor;
}

void ADungeonPlayerController::SetCursor(UDungeonCursorWidget* NewCursor)
{
	Cursor = NewCursor;
}

UDraggableItemWidget* ADungeonPlayerController::GetDraggedItem()
{
	return DraggedItem;
}

void ADungeonPlayerController::StartDraggingItem(UDraggableItemWidget* DraggableItemWidget)
{
	if (Cursor)
	{
		Cursor->Hide();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ADungeonPlayerController::StartDraggingItem - No cursor bound to %s"), *GetName());
	}
	
	if (DraggedItem)
	{
		OnEndItemDrag.Broadcast(DraggedItem->GetItem());
	}
	
	DraggedItem = DraggableItemWidget;
	if (DraggedItem)
	{
		OnBeginItemDrag.Broadcast(DraggedItem->GetItem());
	}

	ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD)
	{
		DungeonHUD->StartDragAndDropOperation(DraggableItemWidget->GetItem());
	}
}

void ADungeonPlayerController::StopDraggingItem()
{
	if (Cursor)
	{
		Cursor->Show();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ADungeonPlayerController::StopDraggingItem - No cursor bound to %s"), *GetName());
	}

	if (DraggedItem)
	{
		OnEndItemDrag.Broadcast(DraggedItem->GetItem());
	}

	ADungeonHUD* DungeonHUD = Cast<ADungeonHUD>(GetHUD());
	if (DungeonHUD)
	{
		DungeonHUD->StopDragAndDropOperation();
	}
}

void ADungeonPlayerController::Server_SetFocusedInteractable_Implementation(AActor* Interactable)
{
	FocusedInteractable = Interactable;
}

bool ADungeonPlayerController::Server_SetFocusedInteractable_Validate(AActor* Actor)
{
	return true;
}

void ADungeonPlayerController::SetPawnCanLook(bool bCanLook)
{
	ADungeonCharacter* DungeonPawn = Cast<ADungeonCharacter>(GetPawn());
	if (DungeonPawn)
	{
		DungeonPawn->SetIsCameraInputEnabled(bCanLook);
	}
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
				IInteractable::Execute_OnUnfocused(FocusedInteractable);
				// Throw event for UI updates
				if (OnInteractableUnfocused.IsBound())
				{
					//OnInteractableUnfocused.Broadcast();
				}
				FocusedInteractable = nullptr;
				Server_SetFocusedInteractable(nullptr);
			}
			return;
		}

		IInteractable* InteractableInterface = nullptr;
		AActor* ClosestInteractable = nullptr;

		FHitResult LineTraceOutHit;
		TArray<FHitResult> SphereTraceOutHits;

		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FVector ControlForwadVector = GetControlRotation().Vector();
		FVector CameraForwardTraceStartLocation = PlayerCameraManager->GetCameraLocation() + (ControlForwadVector * InteractionCameraTraceForwardOffset);
		FVector CameraForwardTraceEndLocation = CameraForwardTraceStartLocation + (ControlForwadVector * InteractionCameraTraceDistance);

		// Line trace for all objects
		FCollisionObjectQueryParams LineQueryObjectParams = FCollisionObjectQueryParams();
		FCollisionQueryParams LineQueryParams;
		LineQueryParams.AddIgnoredActor(GetPawn());

		// Sphere trace only for interactables
		FCollisionObjectQueryParams SphereQueryObjectParams;
		SphereQueryObjectParams.AddObjectTypesToQuery(TRACE_INTERACTABLE);

		// First, do a simple line trace outward from the camera, to get anything the player might be directly targeting.
		bool DidCameraLineTraceHit = GetWorld()->LineTraceSingleByObjectType(LineTraceOutHit, CameraForwardTraceStartLocation, CameraForwardTraceEndLocation, LineQueryObjectParams, LineQueryParams);
		if (DebugInteraction && !DidCameraLineTraceHit)
		{
			// Draw the full line trace with no hit
			DrawDebugLine(GetWorld(), CameraForwardTraceStartLocation, CameraForwardTraceEndLocation, FColor::Orange);
		}
		if (DidCameraLineTraceHit)
		{
			AActor* HitActor = LineTraceOutHit.GetActor();

			// Verify that the actor is within the max interaction range
			float HitDistance = FVector::Distance(HitActor->GetActorLocation(), PlayerPawn->GetActorLocation());
			if (HitDistance <= MaxInteractionDistance)
			{
				InteractableInterface = Cast<IInteractable>(HitActor);
				if (InteractableInterface && InteractableInterface->Execute_GetCanInteract(HitActor))
				{
					if (DebugInteraction)
					{
						// Draw the line trace up to the impact point and draw a green point at that location, signifying an interactable was hit
						DrawDebugLine(GetWorld(), CameraForwardTraceStartLocation, LineTraceOutHit.ImpactPoint, FColor::Orange);
						DrawDebugPoint(GetWorld(), LineTraceOutHit.ImpactPoint, 10, FColor::Green);
					}
					ClosestInteractable = HitActor;
				}
				else
				{
					if (DebugInteraction)
					{
						// Draw the line trace up to the impact point and draw a red point at that location, signifying an actor was hit, but not an interactable 
						DrawDebugLine(GetWorld(), CameraForwardTraceStartLocation, LineTraceOutHit.ImpactPoint, FColor::Orange);
						DrawDebugPoint(GetWorld(), LineTraceOutHit.ImpactPoint, 10, FColor::Red);
					}

					// If the player isn't directly targeting an interactable, do a sphere trace at the line trace hit location, if it exists
					FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionCameraTraceRadius);
					if (DebugInteraction)
					{
						// Draw the sphere at the impact point of the previous line trace
						DrawDebugSphere(GetWorld(), LineTraceOutHit.ImpactPoint, InteractionCameraTraceRadius, 32, FColor::Orange);
					}
					bool DidPrimarySphereTraceHit = GetWorld()->SweepMultiByObjectType(SphereTraceOutHits, LineTraceOutHit.ImpactPoint, LineTraceOutHit.ImpactPoint, FQuat::Identity, SphereQueryObjectParams, Sphere);
					if (DidPrimarySphereTraceHit)
					{
						// Find hit interactable closest to cursor
						for (FHitResult SphereHit : SphereTraceOutHits)
						{
							AActor* HitActor = SphereHit.GetActor();
							// Verify that the actor is within the max interaction range
							float HitDistance = FVector::Distance(HitActor->GetActorLocation(), PlayerPawn->GetActorLocation());
							if (HitDistance <= MaxInteractionDistance)
							{
								InteractableInterface = Cast<IInteractable>(HitActor);
								if (InteractableInterface && InteractableInterface->Execute_GetCanInteract(HitActor))
								{
									if (DebugInteraction)
									{
										// Draw the impact point on the sphere
										DrawDebugPoint(GetWorld(), SphereHit.ImpactPoint, 10, FColor::Green);
									}
									// Check if this hit is closest to the center of the sphere
									if (ClosestInteractable)
									{
										float HitToCenterDistance = FVector::Distance(SphereHit.Location, HitActor->GetActorLocation());
										float ClosestToCenterDistance = FVector::Distance(SphereHit.Location, ClosestInteractable->GetActorLocation());
										if (HitToCenterDistance < ClosestToCenterDistance)
										{
											ClosestInteractable = HitActor;
										}
									}
									else
									{
										ClosestInteractable = HitActor;
									}
								}
							}
							
						}
					}
				}
			}
			if(!ClosestInteractable)
			{
				if (DebugInteraction)
				{
					// Draw the full line trace with no hit
					DrawDebugLine(GetWorld(), CameraForwardTraceStartLocation, CameraForwardTraceEndLocation, FColor::Orange);
				}

				// If there still wasn't a hit, do a complete sphere cast from the camera to initial camera trace hit location. This should hit items that are in front of where the initial line trace hit
				FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionCameraTraceRadius);
				bool DidSecondarySphereTraceHit = GetWorld()->SweepMultiByObjectType(SphereTraceOutHits, CameraForwardTraceStartLocation, LineTraceOutHit.ImpactPoint, FQuat::Identity, SphereQueryObjectParams, Sphere);
				bool HitSphereDrawn = false;
				if (DidSecondarySphereTraceHit)
				{
					// Find hit interactable closest to cursor
					for (FHitResult SphereHit : SphereTraceOutHits)
					{
						if (DebugInteraction && !HitSphereDrawn)
						{
							// Draw the sphere at the impact point of the previous line trace
							DrawDebugSphere(GetWorld(), SphereHit.Location, InteractionCameraTraceRadius, 32, FColor::Orange);
							HitSphereDrawn = true;
						}
						AActor* HitActor = SphereHit.GetActor();
						// Verify that the actor is within the max interaction range
						float HitDistance = FVector::Distance(HitActor->GetActorLocation(), PlayerPawn->GetActorLocation());
						if (HitDistance <= MaxInteractionDistance)
						{
							InteractableInterface = Cast<IInteractable>(HitActor);
							if (InteractableInterface && InteractableInterface->Execute_GetCanInteract(HitActor))
							{
								if (DebugInteraction)
								{
									// Draw the impact point on the sphere
									DrawDebugPoint(GetWorld(), SphereHit.ImpactPoint, 10, FColor::Green);
								}
								// Check if this hit is closest to the center of the sphere
								if (ClosestInteractable)
								{
									float HitToCenterDistance = FVector::Distance(SphereHit.Location, HitActor->GetActorLocation());
									float ClosestToCenterDistance = FVector::Distance(SphereHit.Location, ClosestInteractable->GetActorLocation());
									if (HitToCenterDistance < ClosestToCenterDistance)
									{
										ClosestInteractable = HitActor;
									}
								}
								else
								{
									ClosestInteractable = HitActor;
								}
							}
						}
					}
				}
			}
		}

		// If interactable was found, set as focus and render outline
		if (ClosestInteractable)
		{
			InteractableInterface = Cast<IInteractable>(ClosestInteractable);
			if (FocusedInteractable)
			{
				if (FocusedInteractable == ClosestInteractable)
				{
					return;
				}
				InteractableInterface->Execute_OnUnfocused(FocusedInteractable);
				if (DebugInteraction)
				{
					UE_LOG(LogTemp, Log, TEXT("ADungeonPlayerController::CheckFocus - Stopped focusing %s"), *FocusedInteractable->GetName());
				}
			}
			Server_SetFocusedInteractable(ClosestInteractable);
			FocusedInteractable = ClosestInteractable;
			InteractableInterface->Execute_OnFocused(FocusedInteractable);
			// Throw event for UI updates
			if (OnInteractableFocused.IsBound())
			{
				//OnInteractableFocused.Broadcast(FocusedInteractable);
			}
			if (DebugInteraction)
			{
				UE_LOG(LogTemp, Log, TEXT("ADungeonPlayerController::CheckFocus - Started focusing %s"), *FocusedInteractable->GetName());
			}
		}
		else
		{
			if (FocusedInteractable)
			{
				InteractableInterface = Cast<IInteractable>(FocusedInteractable);
				InteractableInterface->Execute_OnUnfocused(FocusedInteractable);
				// Throw event for UI updates
				if (OnInteractableUnfocused.IsBound())
				{
					//OnInteractableUnfocused.Broadcast();
				}
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
