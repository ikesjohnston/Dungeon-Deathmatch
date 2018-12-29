// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponGlobals.generated.h"

/* Used to determine what equipment slot a weapon should go into when equipping an item, as well as what default character animations should be used. */
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	OneHand		UMETA(DisplayName = "One Hand"),
	TwoHand		UMETA(DisplayName = "Two Hand"),
	OffHand		UMETA(DisplayName = "Off Hand"),
};

/* Used on tooltips and for applying class specific effects on attack. */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	FistWeapon		UMETA(DisplayName = "Fist Weapon"),
	OneHandedSword	UMETA(DisplayName = "One Handed Sword"),
	OneHandedAxe	UMETA(DisplayName = "One Handed Axe"),
	OneHandedMace	UMETA(DisplayName = "One Handed Mace"),
	TwoHandedSword	UMETA(DisplayName = "Two Handed Sword"),
	TwoHandedAxe	UMETA(DisplayName = "Two Handed Axe"),
	TwoHandedMace	UMETA(DisplayName = "Two Handed Mace"),
	Staff			UMETA(DisplayName = "Staff"),
	Polearm			UMETA(DisplayName = "Polearm"),

	NUM_WEAPON_TYPES
};

//
//USTRUCT(BlueprintType)
//struct FWeaponConfig
//{
//	GENERATED_USTRUCT_BODY();
//
//	UPROPERTY()
//	TArray<EWeaponSlot> ValidEquipmentSlots;
//};
//
///**
// * 
// */
//class DUNGEONDEATHMATCH_API UWeaponGlobals : UObject
//{
//	GENERATED_BODY()
//
//public:
//
//	UWeaponGlobals();
//
//	~UWeaponGlobals();
//};
