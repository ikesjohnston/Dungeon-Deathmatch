// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ChestLoot.h"

UAnimNotify_ChestLoot::UAnimNotify_ChestLoot(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotify_ChestLoot::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	AChest* Chest = Cast<AChest>(MeshComp->GetOwner());
	if (Chest && Chest->Role == ROLE_Authority)
	{
		Chest->Loot();
	}
}
