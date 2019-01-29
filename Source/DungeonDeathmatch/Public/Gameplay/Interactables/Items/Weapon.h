// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equippable.h"
#include "EquipmentGlobals.h"
#include "Weapon.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;

/**
 * The base class for all weapons in the game. Stores damaging effects and generates hit events for melee weapons when they are set in an attacking state.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AWeapon : public AEquippable
{
	GENERATED_BODY()
	
protected:
	/* The visual representation of this  weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* WeaponMeshComponent;

	/* The volume that generates overlap events, allowing a weapon to damage a target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UCapsuleComponent* DamagingVolume;

	/** What hand(s) the weapon requires to use. Determines where the weapon is stored on the character and loadout validity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponHand WeaponHand;

	/** The weapon's type. Determines base animations and loadout validity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;

	/** The combat state of the weapon, determines when it is able to be used and do damage */
	EWeaponState WeaponState;

	/** The socket type this weapon uses when sheathed and drawn. Determined by the equipment slot it is in. Determines the sockets to attach the weapon to when equipped. */
	UPROPERTY(Replicated)
	EWeaponSocketType WeaponSocketType;

	/** The relative position adjustment for this weapon when attached to its sheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FVector SheathedSocketPositionAdjustment;

	/** The relative rotation adjustment for this weapon when attached to its sheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FRotator SheathedSocketRotationAdjustment;

	/** The relative position adjustment for this weapon when attached to its unsheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FVector UnsheathedSocketPositionAdjustment;

	/** The relative rotation adjustment for this weapon when attached to its unsheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FRotator UnsheathedSocketRotationAdjustment;

public:
	// Sets default values for this actor's properties
	AWeapon(const FObjectInitializer& ObjectInitializer);

	virtual ~AWeapon() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Gets the hand(s) required to use this weapon */
	EWeaponHand GetWeaponHand();

	/** Gets the type of the weapon */
	EWeaponType GetWeaponType();

	/** Gets the combat state of the weapon */
	EWeaponState GetWeaponState();

	/** Gets The location where this weapon is sheathed. */
	EWeaponSocketType GetWeaponSocketType();

protected:
	// ------------------------ BEGIN EQUIPPABLE OVERRIDES ------------------------
	virtual void ServerOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot) override;
	virtual void MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot) override;

	virtual void ServerOnUnequip_Implementation() override;
	virtual void MulticastOnUnequip_Implementation() override;
	// ------------------------ BEGIN EQUIPPABLE OVERRIDES ------------------------
};
