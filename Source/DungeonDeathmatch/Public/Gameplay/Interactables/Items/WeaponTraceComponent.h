// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponTraceComponent.generated.h"

class AWeapon;

/** Used to determine how to react to different hits. */
UENUM(BlueprintType)
enum class EWeaponTraceType : uint8
{
	Edge		UMETA(DisplayName = "Edge"),
	Center		UMETA(DisplayName = "Center"),
	Base		UMETA(DisplayName = "Base")
};

/** Struct that stores weapon hit information */
USTRUCT(BlueprintType)
struct FWeaponHitResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponTraceType WeaponTraceType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHitResult HitResult;

	FWeaponHitResult()
	{
		HitResult = FHitResult();
	}

	FWeaponHitResult(EWeaponTraceType TraceType, FHitResult Hit)
	{
		WeaponTraceType = TraceType;
		HitResult = Hit;
	}
};

/**
 * Scene component that, when attached to a weapon actor, performs a ray trace every frame to check for collisions, and raises events on the weapon when there is a hit
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UWeaponTraceComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	/** The weapon actor that this trace component is attached to, events will be called on this actor in the event of a collision */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	AWeapon* OwningWeapon;

	/** The type of trace for this component to perform, used to determine how to react to different hits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	EWeaponTraceType TraceType;

	/** The mapping of trace types to colors to render the debug line traces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace\|Debug")
	TMap<EWeaponTraceType, FColor> DebugTraceColors;

	/** The size of the debug point to draw for weapon hits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace\|Debug")
	float DebugHitPointSize;

	/** The time, in seconds, to render the debug line trace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace\|Debug")
	float DebugTraceTime;

	/** Is the component currently making line traces for hits? */
	bool bIsHitTracing;

private:
	/** The end location of the trace from the previous frame*/
	FVector PreviousTraceEndLocation;

public:	
	// Sets default values for this component's properties
	UWeaponTraceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetIsHitTracing(bool IsHitTracing);
};
