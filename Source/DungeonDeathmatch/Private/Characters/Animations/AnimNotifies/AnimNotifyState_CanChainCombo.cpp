// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_CanChainCombo.h"
#include "DungeonCharacter.h"

UAnimNotifyState_CanChainCombo::UAnimNotifyState_CanChainCombo(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotifyState_CanChainCombo::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->Server_SetAttackInProgress(false);
		Character->Server_IncreaseCombo();
	}
}

void UAnimNotifyState_CanChainCombo::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->Server_ResetCombo();
	}
}

void UAnimNotifyState_CanChainCombo::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
