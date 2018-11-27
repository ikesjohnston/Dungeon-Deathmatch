// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_MeleeCanDamage.h"
#include "DungeonCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/SphereComponent.h>

UAnimNotifyState_MeleeCanDamage::UAnimNotifyState_MeleeCanDamage(const FObjectInitializer& ObjectInitializer)
{
	bIsMainHandAttack = true;
	bIsUnarmedAttack = false;
}

void UAnimNotifyState_MeleeCanDamage::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		if (bIsUnarmedAttack)
		{
			// Enable overlap events for fists
			if (bIsMainHandAttack)
			{
				Character->GetRightFistCollider()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			else
			{
				Character->GetLeftFistCollider()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
	}
}

void UAnimNotifyState_MeleeCanDamage::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		if (bIsUnarmedAttack)
		{
			// Disable overlap events for fists
			if (bIsMainHandAttack)
			{
				Character->GetRightFistCollider()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				Character->GetLeftFistCollider()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void UAnimNotifyState_MeleeCanDamage::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
