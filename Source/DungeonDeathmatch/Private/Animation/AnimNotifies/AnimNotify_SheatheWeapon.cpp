// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SheatheWeapon.h"
#include "DungeonCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include "EquipmentComponent.h"
#include "Weapon.h"

UAnimNotify_SheatheWeapon::UAnimNotify_SheatheWeapon(const FObjectInitializer& ObjectInitializer)
{
	bSheatheOffHand = false;
}

void UAnimNotify_SheatheWeapon::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	ADungeonCharacter* Character = Cast<ADungeonCharacter>(MeshComp->GetOwner());
	if (Character && Character->Role == ROLE_Authority)
	{
		UEquipmentComponent* EquipmentComponent = Character->GetEquipmentComponent();
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveWeaponLoadout = EquipmentComponent->GetActiveWeaponLoadout();
			AWeapon* WeaponToSheathe = nullptr;

			if (bSheatheOffHand)
			{
				WeaponToSheathe = ActiveWeaponLoadout.OffHandWeapon;
			}
			else
			{
				WeaponToSheathe = ActiveWeaponLoadout.MainHandWeapon;
			}

			if (WeaponToSheathe)
			{
				EWeaponSocketType WeaponSocketType = WeaponToSheathe->GetWeaponSocketType();
				FName AttachSocketName = Character->GetNameForWeaponSocket(WeaponSocketType);
				FVector AttachPositionAdjustment = WeaponToSheathe->GetSheathedSocketPositionAdjustment();
				FRotator AttachRotationAdjustment = WeaponToSheathe->GetSheathedSocketRotationAdjustment();
				Character->Server_DetachActor(WeaponToSheathe);
				Character->Server_AttachActorToSocket(WeaponToSheathe, AttachSocketName, AttachPositionAdjustment, AttachRotationAdjustment);
				Character->Server_SetCombatState(ECombatState::Sheathed);
			}
		}
	}
}
