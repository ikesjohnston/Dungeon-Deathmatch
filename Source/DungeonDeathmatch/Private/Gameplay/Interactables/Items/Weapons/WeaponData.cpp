#include "WeaponData.h"

UWeaponData::UWeaponData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DrawnAttachSocketName = NAME_None;
	SheathedAttachSocketName = NAME_None;
}

const FName& UWeaponData::GetDrawnAttachSocketName() const
{
	return DrawnAttachSocketName;
}

const FName& UWeaponData::GetSheathedAttachSocketName() const
{
	return SheathedAttachSocketName;
}

UAnimMontage* UWeaponData::GetDrawWeaponMontage() const
{
	return DrawWeaponMontage;
}

UAnimMontage* UWeaponData::GetSheatheWeaponMontage() const
{
	return SheatheWeaponMontage;
}

UAnimMontage* UWeaponData::GetAttackOneMontage() const
{
	return AttackOneMontage;
}

UAnimMontage* UWeaponData::GetAttackTwoMontage() const
{
	return AttackTwoMontage;
}

UAnimMontage* UWeaponData::GetAttackThreeMontage() const
{
	return AttackThreeMontage;
}
