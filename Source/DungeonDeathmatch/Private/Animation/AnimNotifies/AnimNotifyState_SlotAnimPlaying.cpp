// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_SlotAnimPlaying.h"
#include <Components/SkeletalMeshComponent.h>
#include "DungeonCharacterAnimInstance.h"

UAnimNotifyState_SlotAnimPlaying::UAnimNotifyState_SlotAnimPlaying(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotifyState_SlotAnimPlaying::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	UDungeonCharacterAnimInstance* AnimInstance = Cast<UDungeonCharacterAnimInstance>(MeshComp->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->SetMontageStartedInSlot(AnimSlot);
	}
}

void UAnimNotifyState_SlotAnimPlaying::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	UDungeonCharacterAnimInstance* AnimInstance = Cast<UDungeonCharacterAnimInstance>(MeshComp->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->SetMontageStoppedInSlot(AnimSlot);
	}
}

void UAnimNotifyState_SlotAnimPlaying::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
