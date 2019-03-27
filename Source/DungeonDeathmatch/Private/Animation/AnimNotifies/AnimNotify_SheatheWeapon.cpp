// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SheatheWeapon.h"
#include "EquipmentComponent.h"
#include "Weapon.h"
#include "PlayerCombatComponent.h"

#include <Components/SkeletalMeshComponent.h>


UAnimNotify_SheatheWeapon::UAnimNotify_SheatheWeapon(const FObjectInitializer& ObjectInitializer)
{
	bSheatheOffHand = false;
}

void UAnimNotify_SheatheWeapon::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	APawn* Pawn = Cast<APawn>(MeshComp->GetOwner());
	if (Pawn && Pawn->HasAuthority())
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(Pawn->GetComponentByClass(UEquipmentComponent::StaticClass()));
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
				/*FName AttachSocketName = Character->GetNameForWeaponSocket(WeaponSocketType);
				FVector AttachPositionAdjustment = WeaponToSheathe->GetSheathedSocketPositionAdjustment();
				FRotator AttachRotationAdjustment = WeaponToSheathe->GetSheathedSocketRotationAdjustment();
				Character->ServerDetachActor(WeaponToSheathe);
				Character->ServerAttachActorToSocket(WeaponToSheathe, AttachSocketName, AttachPositionAdjustment, AttachRotationAdjustment);
				Character->GetCombatComponent()->ServerSetCombatState(ECombatState::Sheathed);*/
			}
		}
	}
}
