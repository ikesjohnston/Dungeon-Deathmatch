#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Head			UMETA(DisplayName = "Head"),
	Shoulders		UMETA(DisplayName = "Shoulders"),
	Chest			UMETA(DisplayName = "Chest"),
	Hands			UMETA(DisplayName = "Hands"),
	Waist			UMETA(DisplayName = "Waist"),
	Legs			UMETA(DisplayName = "Legs"),
	Feet			UMETA(DisplayName = "Feet"),

	NUM_EQUIPMENT_SLOTS
};

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Head			UMETA(DisplayName = "Head"),
	Shoulders		UMETA(DisplayName = "Shoulders"),
	Chest			UMETA(DisplayName = "Chest"),
	Waist			UMETA(DisplayName = "Waist"),
	Legs			UMETA(DisplayName = "Legs"),
	Feet			UMETA(DisplayName = "Feet"),
	Hands			UMETA(DisplayName = "Hands"),

	NUM_ARMOR_SLOTS UMETA(Hidden)
};

/* Used to determine what equipment slot a weapon can go into when equipping an item. */
UENUM(BlueprintType)
enum class EWeaponHand : uint8
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
	Bow				UMETA(DisplayName = "Bow"),
	Crossbow		UMETA(DisplayName = "Crossbow"),
	Shield			UMETA(DisplayName = "Shield"),

	NUM_WEAPON_TYPES
};

/*  Used in combat logic to determine if a weapon is available for use. */
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Sheathed			UMETA(DisplayName = "Weapon Sheathed"),
	Unsheathing			UMETA(DisplayName = "Unsheathing Weapon"),
	Sheathing			UMETA(DisplayName = "Sheathing Weapon"),
	ReadyToUse			UMETA(DisplayName = "WeaponReady"),
	AttackInProgress	UMETA(DisplayName = "Attacking")
};

/** Represents a specific socket on a character to place an individual weapon */
UENUM(BlueprintType)
enum class EWeaponSheatheLocation : uint8
{
	WaistOneHandOne		UMETA(DisplayName = "Waist One Hand One"),
	WasitOneHandTwo		UMETA(DisplayName = "Wasit One Hand Two"),
	BackOneHandOne		UMETA(DisplayName = "Back One Hand One"),
	BackOneHandTwo		UMETA(DisplayName = "Back One Hand Two"),
	BackOffHandOne		UMETA(DisplayName = "Back Off Hand One"),
	BackOffHandTwo		UMETA(DisplayName = "Back Off Hand Two"),
	BackTwoHandOne		UMETA(DisplayName = "Back Two Hand One"),
	BackTwoHandTwo		UMETA(DisplayName = "Back Two Hand Two")
};

/** Represents a specific set of sockets on a character to place the weapons of a loadout */
UENUM(BlueprintType)
enum class ELoadoutSheatheLocation : uint8
{
	DualWieldOne		UMETA(DisplayName = "Dual Wield One"),
	DualWieldOneTwo		UMETA(DisplayName = "Dual Wield Two"),
	OneHandShieldOne	UMETA(DisplayName = "One Hand & Shield One"),
	OneHandShieldTwo	UMETA(DisplayName = "One Hand & Shield Two"),
	TwoHandOne			UMETA(DisplayName = "Two Hand One"),
	TwoHandTwo			UMETA(DisplayName = "Two Hand Two")
};

/** Represents a specific hand that is being requested to equip or unequip a weapon */
UENUM(BlueprintType)
enum class ERequestedHand : uint8
{
	MainHand	UMETA(DisplayName = "Main Hand"),
	OffHand		UMETA(DisplayName = "Off Hand"),
	Both		UMETA(DisplayName = "Both")
};