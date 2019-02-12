// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_SwingWeapon.h"
#include "DungeonCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include "EquipmentComponent.h"
#include "Weapon.h"

UAnimNotifyState_SwingWeapon::UAnimNotifyState_SwingWeapon(const FObjectInitializer& ObjectInitializer)
{
}

void UAnimNotifyState_SwingWeapon::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UEquipmentComponent* EquipmentComponent = Character->GetEquipmentComponent();
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveWeaponLoadout = EquipmentComponent->GetActiveWeaponLoadout();
			SwingingWeapon = nullptr;

			if (bSwingOffHand)
			{
				SwingingWeapon = ActiveWeaponLoadout.OffHandWeapon;
			}
			else
			{
				SwingingWeapon = ActiveWeaponLoadout.MainHandWeapon;
			}

			if (SwingingWeapon->IsValidLowLevelFast())
			{
				SwingingWeapon->StartSwing();
				// TODO: Trigger allow damage here
			}
		}
	}
}

void UAnimNotifyState_SwingWeapon::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	if (SwingingWeapon->IsValidLowLevelFast())
	{
		SwingingWeapon->StopSwing();
		// TODO: End allow damage here
	}
}

void UAnimNotifyState_SwingWeapon::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}
