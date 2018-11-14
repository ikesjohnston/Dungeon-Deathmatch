// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Equippable.h"
#include "Weapon.generated.h"

class UCapsuleComponent;
class UWeaponData;

/*
 * Weapon Class Enum
 * Used to determine what equipment slot a weapon should go into
 * when equipping an item. Also used for tooltips.
 */
UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	FistWeapon		UMETA(DisplayName = "Fist Weapon"),
	OneHanded		UMETA(DisplayName = "One Handed"),
	TwoHanded		UMETA(DisplayName = "Two Handed"),
	Bow				UMETA(DisplayName = "Bow"),
	Shield			UMETA(DisplayName = "Shield"),
	Spell			UMETA(DisplayName = "Spell")
};

/*
 * Weapon State Enum
 * Used in combat logic to determine if a weapon is available for use.
 */
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Sheathed			UMETA(DisplayName = "Weapon Sheathed"),
	Unsheathing			UMETA(DisplayName = "Unsheathing Weapon"),
	Sheathing			UMETA(DisplayName = "Sheathing Weapon"),
	ReadyToUse			UMETA(DisplayName = "WeaponReady"),
	AttackInProgress	UMETA(DisplayName = "Attacking")
};

/*
 * Weapon Class
 * The base class for all weapons in the game. Stores damage stats and
 * generates hit events for melee weapons when they are set in an attacking
 * state.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AWeapon : public AItem, public IEquippable
{
	GENERATED_BODY()
	
public:
	/* 
	 * The volume that generates overlap events stating that a weapon can damage
	 * a target.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UCapsuleComponent* DamagingVolume;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UWeaponData* WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponClass WeaponClass;

	EWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 BaseDamageMin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 BaseDamageMax;

public:
	// Sets default values for this actor's properties
	AWeapon();
	virtual ~AWeapon() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UWeaponData* GetWeaponData();

	EWeaponClass GetWeaponClass();

	EWeaponState GetWeaponState();

private:
	virtual void Equip(ADungeonCharacter* Character) override;


	virtual void OnEquip(ADungeonCharacter* Character) override;

};
