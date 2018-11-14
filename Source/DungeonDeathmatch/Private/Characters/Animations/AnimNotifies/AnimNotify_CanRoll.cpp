// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_CanRoll.h"
#include "DungeonCharacter.h"




UAnimNotify_CanRoll::UAnimNotify_CanRoll(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotify_CanRoll::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetCanRoll(true);
	}
}
