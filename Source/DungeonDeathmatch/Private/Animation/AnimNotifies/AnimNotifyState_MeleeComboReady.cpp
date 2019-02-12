// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_MeleeComboReady.h"
#include "DungeonCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include "EquipmentComponent.h"
#include "Weapon.h"

UAnimNotifyState_MeleeComboReady::UAnimNotifyState_MeleeComboReady(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotifyState_MeleeComboReady::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character && Character->Role == ROLE_Authority)
	{
		Character->Multicast_IncrementMeleeComboCounter();
	}
}

void UAnimNotifyState_MeleeComboReady::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character && Character->Role == ROLE_Authority)
	{
		Character->Multicast_ClearMeleeComboCounter();
		Character->Multicast_SetActiveMeleeComboType(EMeleeComboType::None);
	}
}

void UAnimNotifyState_MeleeComboReady::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}