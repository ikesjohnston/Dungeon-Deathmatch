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
		UAISense_Hearing::ReportNoiseEvent(Owner->GetWorld(), NoiseLocation, Loudness, Owner, MaxRange);
	}
}
