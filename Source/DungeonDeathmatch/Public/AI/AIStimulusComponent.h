// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AIStimulusComponent.generated.h"

/**
 * Component for encapsulating variables for the generation of stimuli to be detected by the AI Perception System
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UAIStimulusComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Stimulus|Noise")
	float NoiseMaxRange = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Stimulus|Noise")
	float BaseStandardNoiseLoudness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Stimulus|Noise")
	float BaseStealthedNoiseLoudness = 0.5f;

public:	
	// Sets default values for this component's properties
	UAIStimulusComponent();	

	UFUNCTION(BlueprintPure)
	float GetNoiseMaxRange();

	UFUNCTION(BlueprintPure)
	float GetNoiseLoudness(bool IsStealthed = false);
};
