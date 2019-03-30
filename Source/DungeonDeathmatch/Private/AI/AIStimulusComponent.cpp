// Fill out your copyright notice in the Description page of Project Settings.

#include "AIStimulusComponent.h"
#include "EquipmentComponent.h"

UAIStimulusComponent::UAIStimulusComponent()
{

}

float UAIStimulusComponent::GetNoiseMaxRange()
{
	return NoiseMaxRange;
}

float UAIStimulusComponent::GetNoiseLoudness(bool IsStealthed /*= false*/)
{
	float Loudness = IsStealthed ? BaseStealthedNoiseLoudness : BaseStandardNoiseLoudness;
	UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(GetOwner()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (EquipmentComponent)
	{
		// TODO: Modify loudness based on equipment
	}
	return Loudness;
}

