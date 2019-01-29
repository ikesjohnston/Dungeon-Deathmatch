// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include "EquipmentGlobals.h"
#include "Equippable.h"

// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	DamagingVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DamagingVolume"));
	DamagingVolume->SetupAttachment(RootMeshComponent);
}

AWeapon::~AWeapon()
{

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponSocketType);
}

EWeaponHand AWeapon::GetWeaponHand()
{
	return WeaponHand;
}

EWeaponType AWeapon::GetWeaponType()
{
	return WeaponType;
}

EWeaponState AWeapon::GetWeaponState()
{
	return WeaponState;
}

EWeaponSocketType AWeapon::GetWeaponSocketType()
{
	return WeaponSocketType;
}

void AWeapon::ServerOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::ServerOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);

	switch (EquipmentSlot)
	{
	case EEquipmentSlot::WeaponLoadoutOneMainHand:
		if (WeaponHand == EWeaponHand::OneHand)
		{
			WeaponSocketType = EWeaponSocketType::MainHandWeaponLoadoutOne;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::TwoHandWeaponLoadoutOne;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutOneOffHand:
		if (WeaponType == EWeaponType::Shield)
		{
			WeaponSocketType = EWeaponSocketType::OffHandShieldLoadoutOne;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::OffHandWeaponLoadoutOne;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutTwoMainHand:
		if (WeaponHand == EWeaponHand::OneHand)
		{
			WeaponSocketType = EWeaponSocketType::MainHandWeaponLoadoutTwo;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::TwoHandWeaponLoadoutTwo;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutTwoOffHand:
		if (WeaponType == EWeaponType::Shield)
		{
			WeaponSocketType = EWeaponSocketType::OffHandShieldLoadoutTwo;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::OffHandWeaponLoadoutTwo;
		}
		break;
	default:
		break;
	}

	FName AttachSocketName = NewEquippingCharacter->GetNameForWeaponSocket(WeaponSocketType);
	NewEquippingCharacter->Server_AttachActorToSocket(this, AttachSocketName, UnsheathedSocketPositionAdjustment, UnsheathedSocketRotationAdjustment);
}

void AWeapon::MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);
}

void AWeapon::ServerOnUnequip_Implementation()
{
	EquippingCharacter->Server_DetachActor(this);
	Super::ServerOnUnequip_Implementation();
}

void AWeapon::MulticastOnUnequip_Implementation()
{
	Super::MulticastOnUnequip_Implementation();
}

//void AWeapon::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
//{
//	Super::OnEquip(NewEquippingCharacter);
//
//	/*UMeshComponent* RootMeshComponent = GetRootMeshComponent();
//	if (RootMeshComponent)
//	{
//		RootMeshComponent->SetSimulatePhysics(false);
//		RootMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		RootMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
//	}*/
//}
//
//void AWeapon::OnUnequip_Implementation()
//{
//	//if (Role == ROLE_Authority)
//	//{
//	//	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//	//	bool bWasReturnedToInventory = EquippingCharacter->TryAddItemToInventory(this);
//	//	// If the item wasn't able to be picked up, just drop it
//	//	if (!bWasReturnedToInventory)
//	//	{
//	//		GetMeshComponent()->SetSimulatePhysics(true);
//	//		GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	//		GetMeshComponent()->SetCollisionObjectType(TRACE_INTERACTABLE);
//	//		GetMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
//	//	}
//	//	Super::OnUnequip();
//	//}
//}

