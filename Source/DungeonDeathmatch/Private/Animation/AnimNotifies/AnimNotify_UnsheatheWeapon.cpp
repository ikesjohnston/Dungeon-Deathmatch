// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_UnsheatheWeapon.h"
#include "EquipmentComponent.h"
#include "Weapon.h"
#include "PlayerCombatComponent.h"

#include <Components/SkeletalMeshComponent.h>

UAnimNotify_UnsheatheWeapon::UAnimNotify_UnsheatheWeapon(const FObjectInitializer& ObjectInitializer)
{
	bUnsheatheOffHand = false;
}

void UAnimNotify_UnsheatheWeapon::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	APawn* Pawn = Cast<APawn>(MeshComp->GetOwner());
	if (Pawn && Pawn->HasAuthority())
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(Pawn->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveWeaponLoadout = EquipmentComponent->GetActiveWeaponLoadout();
			AWeapon* WeaponToSheathe = nullptr;
			FName AttachSocketName = NAME_None;
			if (bUnsheatheOffHand)
			{
				WeaponToSheathe = ActiveWeaponLoadout.OffHandWeapon;
				AttachSocketName = FName("OffHand");
			}
			else
			{
				WeaponToSheathe = ActiveWeaponLoadout.MainHandWeapon;
				AttachSocketName = FName("MainHand");
			}

			if (WeaponToSheathe)
			{
				/*EWeaponSocketType WeaponSocketType = WeaponToSheathe->GetWeaponSocketType();
				FVector AttachPositionAdjustment = WeaponToSheathe->GetUnsheathedSocketPositionAdjustment();
				FRotator AttachRotationAdjustment = WeaponToSheathe->GetUnsheathedSocketRotationAdjustment();
				Character->ServerDetachActor(WeaponToSheathe);
				Character->ServerAttachActorToSocket(WeaponToSheathe, AttachSocketName, AttachPositionAdjustment, AttachRotationAdjustment);
				Character->GetCombatComponent()->ServerSetCombatState(ECombatState::ReadyToUse);*/
			}
		}
	}
}
