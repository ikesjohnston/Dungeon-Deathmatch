// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_RollComplete.h"
#include "DungeonCharacter.h"




UAnimNotifyState_RollComplete::UAnimNotifyState_RollComplete(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotifyState_RollComplete::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->EndRoll();
	}
}

void UAnimNotifyState_RollComplete::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{

}

void UAnimNotifyState_RollComplete::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
