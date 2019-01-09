// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>
#include "Equippable.h"

// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	DamagingVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damaging Volume"));
	DamagingVolume->SetupAttachment(MeshComponent);
}

AWeapon::~AWeapon()
{

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void AWeapon::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
{
	Super::OnEquip(NewEquippingCharacter);

	GetMeshComponent()->SetSimulatePhysics(false);
	GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMeshComponent()->SetCollisionObjectType(ECC_WorldDynamic);
}

void AWeapon::OnUnequip_Implementation()
{
	if (Role == ROLE_Authority)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bool bWasReturnedToInventory = EquippingCharacter->TryAddItemToInventory(this);
		// If the item wasn't able to be picked up, just drop it
		if (!bWasReturnedToInventory)
		{
			GetMeshComponent()->SetSimulatePhysics(true);
			GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMeshComponent()->SetCollisionObjectType(TRACE_INTERACTABLE);
			GetMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
		Super::OnUnequip();
	}
}

