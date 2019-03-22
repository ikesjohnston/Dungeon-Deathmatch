#include "EquipmentGlobals.h"
#include "Weapon.h"

ELoadoutType UDungeonEquipmentLibrary::GetLoadoutType(const FWeaponLoadout Loadout)
{
	ELoadoutType LoadoutType = ELoadoutType::Unarmed;

	if (Loadout.MainHandWeapon && !Loadout.OffHandWeapon)
	{
		if (Loadout.MainHandWeapon->GetWeaponHand() == EWeaponHand::TwoHand)
		{
			LoadoutType = ELoadoutType::TwoHand;
		}
		else if (Loadout.MainHandWeapon->GetWeaponHand() == EWeaponHand::OneHand)
		{
			LoadoutType = ELoadoutType::OneHand;
		}
	}
	else if (Loadout.OffHandWeapon)
	{
		if (Loadout.OffHandWeapon->GetWeaponType() == EWeaponType::Shield)
		{
			LoadoutType = ELoadoutType::OneHandAndShield;
		}
		else
		{
			LoadoutType = ELoadoutType::DualWield;
		}
	}

	return LoadoutType;
}
