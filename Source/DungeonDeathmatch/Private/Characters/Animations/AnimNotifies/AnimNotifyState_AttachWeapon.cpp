// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_AttachWeapon.h"
#include "EquipmentComponent.h"
#include "Weapon.h"
#include <Engine/EngineTypes.h>
#include <Components/SkeletalMeshComponent.h>

UAnimNotifyState_AttachWeapon::UAnimNotifyState_AttachWeapon(const FObjectInitializer& ObjectInitializer)
{

}

void UAnimNotifyState_AttachWeapon::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		/*UEquipmentComponent* Equipment = Character->EquipmentComponent;
		if (Equipment)
		{
			AWeapon* MainHandWeapon = Equipment->GetEquipmentInSlot(EEquipmentSlot::MainHand);
			if (MainHandWeapon)
			{
				UWeaponData* WeaponData = MainHandWeapon->GetWeaponData();
				if (WeaponData)
				{
					FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
					FName AttachSocketName = WeaponData->GetDrawnAttachSocketName();
					MainHandWeapon->AttachToComponent(MeshComp, AttachRules,)
				}
			}
		}*/
	}
}

void UAnimNotifyState_AttachWeapon::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{

}

void UAnimNotifyState_AttachWeapon::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
