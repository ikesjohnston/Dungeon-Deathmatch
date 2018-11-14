// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "Weapon.h"
#include "WeaponData.generated.h"

/*
 * Weapon Type Data Asset Class
 * Used to store animation and socket data for a particular weapon type,
 * as this information will generally be the same for each class of weapon.
 */
UCLASS()
class UWeaponData : public UDataAsset
{
	GENERATED_BODY()

protected:
	/*
	 * The socket on the character mesh to attach the weapon to when drawn
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment")
	FName DrawnAttachSocketName;

	/*
	 * The socket on the character mesh to attach the weapon to when sheathed
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attachment")
	FName SheathedAttachSocketName;

	/*
	 * The animation montage to play for the weapon to when drawing
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DrawWeaponMontage;

	/*
	 * The animation montage to play for the weapon to when sheathing
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* SheatheWeaponMontage;

	/* 
	 * Until a more advanced melee combo system is put in place, each
	 * weapon will simply have a three attack combo of preset animations
	 */

	/*
	 * The animation montage to play for the first attack in the combo
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackOneMontage;

	/*
	 * The animation montage to play for the second attack in the combo
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackTwoMontage;

	/*
	 * The animation montage to play for the third and final attack in the combo
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackThreeMontage;

public:
	UWeaponData(const FObjectInitializer& ObjectInitializer);

	const FName& GetDrawnAttachSocketName() const;

	const FName& GetSheathedAttachSocketName() const;

	UAnimMontage* GetDrawWeaponMontage() const;

	UAnimMontage* GetSheatheWeaponMontage() const;

	UAnimMontage* GetAttackOneMontage() const;

	UAnimMontage* GetAttackTwoMontage() const;

	UAnimMontage* GetAttackThreeMontage() const;
};