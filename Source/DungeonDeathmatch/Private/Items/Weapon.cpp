// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>

// Sets default values
AWeapon::AWeapon()
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

EWeaponClass AWeapon::GetWeaponClass()
{
	return WeaponClass;
}

EWeaponState AWeapon::GetWeaponState()
{
	return WeaponState;
}

void AWeapon::NativeOnEquip(ADungeonCharacter* EquippingCharacter)
{
	OnEquip(EquippingCharacter);
}

void AWeapon::NativeOnUnequip(ADungeonCharacter* UnequippingCharacter)
{
	OnUnequip(UnequippingCharacter);
}

