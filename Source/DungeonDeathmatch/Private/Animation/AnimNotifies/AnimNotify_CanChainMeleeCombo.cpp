// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_CanChainMeleeCombo.h"
#include "DungeonCharacter.h"

UAnimNotify_CanChainMeleeCombo::UAnimNotify_CanChainMeleeCombo(const FObjectInitializer& ObjectInitializer)
{
	meleeComboOpeningTime = 1.0f;
}

void UAnimNotify_CanChainMeleeCombo::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character && Character->Role == ROLE_Authority)
	{
		Character->Server_IncreaseMeleeComboState();
		Character->Server_BeginMeleeComboEndTimer(meleeComboOpeningTime);
	}
}
