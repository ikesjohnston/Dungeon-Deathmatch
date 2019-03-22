#pragma once

#include "CoreMinimal.h"

/** Used in combat logic to determine if a weapon is available for use. */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Sheathed			UMETA(DisplayName = "Weapon Sheathed"),
	Unsheathing			UMETA(DisplayName = "Unsheathing Weapon"),
	Sheathing			UMETA(DisplayName = "Sheathing Weapon"),
	ReadyToUse			UMETA(DisplayName = "WeaponReady"),
	AttackInProgress	UMETA(DisplayName = "Attacking")
};

/** Used in combat logic to keep track of combo tallies for different attack types. */
UENUM(BlueprintType)
enum class EMeleeComboType : uint8
{
	None			UMETA(DisplayName = "None"),
	MainHand		UMETA(DisplayName = "Main Hand"),
	MainHandAlt		UMETA(DisplayName = "Main Hand Alt"),
	OffHand			UMETA(DisplayName = "Off Hand"),
	OffHandAlt		UMETA(DisplayName = "Off Hand Alt")
};