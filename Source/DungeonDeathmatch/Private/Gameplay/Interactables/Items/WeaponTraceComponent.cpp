// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponTraceComponent.h"
#include <CollisionQueryParams.h>
#include <Components/ActorComponent.h>
#include <DrawDebugHelpers.h>
#include "Weapon.h"

// Console command for drawing weapon swing line traces
static int32 DebugWeaponTracing = 0;
FAutoConsoleVariableRef CVARDebugWeaponTracing(
	TEXT("Dungeon.DebugWeaponTracing"),
	DebugWeaponTracing,
	TEXT("Draw lines for weapon swing traces"),
	ECVF_Cheat);

// Sets default values for this component's properties
UWeaponTraceComponent::UWeaponTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DebugTraceColors.Add(EWeaponTraceType::Edge, FColor::Green);
	DebugTraceColors.Add(EWeaponTraceType::Center, FColor::Yellow);
	DebugTraceColors.Add(EWeaponTraceType::Base, FColor::Red);

	DebugHitPointSize = 5.0f;

	DebugTraceTime = 1.0f;

	OwningWeapon = Cast<AWeapon>(GetAttachmentRootActor());
	if (!OwningWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWeaponTraceComponent::UWeaponTraceComponent - %s needs to be attached to a Weapon actor to work properly."), *GetName());
	}

	bIsHitTracing = false;
}

// Called when the game starts
void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwningWeapon = Cast<AWeapon>(GetAttachmentRootActor());
	if (!OwningWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWeaponTraceComponent::UWeaponTraceComponent - %s needs to be attached to a Weapon actor to work properly."), *GetName());
	}
	else
	{
		PreviousTraceEndLocation = OwningWeapon->GetActorLocation();
	}
}

// Called every frame
void UWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwningWeapon)
	{
		FHitResult LineTraceOutHit;

		FVector TraceStartLocation = PreviousTraceEndLocation;
		FVector TraceEndLocation = GetComponentLocation();

		if (bIsHitTracing)
		{
			// Line trace for anything that blocks attack traces
			FCollisionQueryParams LineQueryParams;
			LineQueryParams.AddIgnoredActor(OwningWeapon);
			LineQueryParams.AddIgnoredActor(OwningWeapon->EquippingCharacter);
			LineQueryParams.bReturnPhysicalMaterial = true;

			// Do a simple line trace in the component's forward direction
			bool DidWeaponLineTraceHit = GetWorld()->LineTraceSingleByChannel(LineTraceOutHit, TraceStartLocation, TraceEndLocation, TRACE_ATTACK, LineQueryParams);
			if (DebugWeaponTracing)
			{
				// Draw the full line trace with no hit
				FColor DebugColor = FColor::Blue;
				FColor* ColorPtr = DebugTraceColors.Find(TraceType);
				if (ColorPtr)
				{
					DebugColor = *ColorPtr;
				}
				DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, DebugColor, true, DebugTraceTime);
			}
			if (DidWeaponLineTraceHit)
			{
				if (DebugWeaponTracing)
				{
					// Draw a point at the hit location
					FColor DebugColor = FColor::Blue;
					FColor* ColorPtr = DebugTraceColors.Find(TraceType);
					if (ColorPtr)
					{
						DebugColor = *ColorPtr;
					}
					DrawDebugPoint(GetWorld(), LineTraceOutHit.ImpactPoint, DebugHitPointSize, DebugColor, true, DebugTraceTime);
				}

				FWeaponHitResult HitResult = FWeaponHitResult(TraceType, LineTraceOutHit);
				OwningWeapon->OnHitDetected(HitResult);
			}
		}

		PreviousTraceEndLocation = TraceEndLocation;
	}
}

void UWeaponTraceComponent::SetIsHitTracing(bool IsHitTracing)
{
	bIsHitTracing = IsHitTracing;
}

