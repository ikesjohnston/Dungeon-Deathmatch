// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ReportNoise.h"

#include <Perception/AISense_Hearing.h>
#include <Components/ActorComponent.h>

UAnimNotify_ReportNoise::UAnimNotify_ReportNoise(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotify_ReportNoise::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner)
	{
		FVector NoiseLocation = Owner->GetActorLocation();
		if (!SocketName.IsNone())
		{
			NoiseLocation = MeshComp->GetSocketLocation(SocketName);
		}

		float Loudness = 1.0f;
		float MaxRange = 0.0f;
		UAIStimulusComponent* StimulusComponent = Cast<UAIStimulusComponent>(Owner->GetComponentByClass(UAIStimulusComponent::StaticClass()));
		if (StimulusComponent)
		{
			Loudness = StimulusComponent->GetNoiseLoudness(bIsNoiseStealthed);
			MaxRange = StimulusComponent->GetNoiseMaxRange();
		}

		UAISense_Hearing::ReportNoiseEvent(Owner->GetWorld(), NoiseLocation, Loudness, Owner, MaxRange);
	}
}
