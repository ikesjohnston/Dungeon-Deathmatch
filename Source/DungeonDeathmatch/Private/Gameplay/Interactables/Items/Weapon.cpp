// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>

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

UWeaponData* AWeapon::GetWeaponData()
{
	return WeaponData;
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

}

