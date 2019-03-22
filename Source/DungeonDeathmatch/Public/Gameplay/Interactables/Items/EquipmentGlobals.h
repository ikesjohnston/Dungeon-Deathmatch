#pragma once

#include "CoreMinimal.h"
#include <Kismet/BlueprintFunctionLibrary.h>

#include "EquipmentGlobals.generated.h"

class AWeapon;

/** All equipment slots available to a character */
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Head						UMETA(DisplayName = "Head"),
	Neck						UMETA(DisplayName = "Neck"),
	Shoulders					UMETA(DisplayName = "Shoulders"),
	Chest						UMETA(DisplayName = "Chest"),
	Hands						UMETA(DisplayName = "Hands"),
	FingerOne					UMETA(DisplayName = "Finger One"),
	FingerTwo					UMETA(DisplayName = "Finger Two"),
	Waist						UMETA(DisplayName = "Waist"),
	Legs						UMETA(DisplayName = "Legs"),
	Feet						UMETA(DisplayName = "Feet"),
	WeaponLoadoutOneMainHand	UMETA(DisplayName = "Weapon Loadout One - Main Hand"),
	WeaponLoadoutOneOffHand		UMETA(DisplayName = "Weapon Loadout One - Off Hand"),
	WeaponLoadoutTwoMainHand	UMETA(DisplayName = "Weapon Loadout Two - Main Hand"),
	WeaponLoadoutTwoOffHand		UMETA(DisplayName = "Weapon Loadout Two - Off Hand"),

	NUM_EQUIPMENT_SLOTS			UMETA(Hidden)
};

/** Used to determine what equipment slot a piece of armor can go into when equipping an item. */
UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Head			UMETA(DisplayName = "Head"),
	Neck			UMETA(DisplayName = "Neck"),
	Shoulders		UMETA(DisplayName = "Shoulders"),
	Chest			UMETA(DisplayName = "Chest"),
	Hands			UMETA(DisplayName = "Hands"),
	Finger			UMETA(DisplayName = "Finger"),
	Waist			UMETA(DisplayName = "Waist"),
	Legs			UMETA(DisplayName = "Legs"),
	Feet			UMETA(DisplayName = "Feet"),

	NUM_ARMOR_SLOTS UMETA(Hidden)
};

/** Used to determine what equipment slot a weapon can go into when equipping an item. */
UENUM(BlueprintType)
enum class EWeaponHand : uint8
{
	OneHand		UMETA(DisplayName = "One Hand"),
	TwoHand		UMETA(DisplayName = "Two Hand"),
	OffHand		UMETA(DisplayName = "Off Hand"),
};

/** Used on tooltips and for applying class specific effects on attack. */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	FistWeapon		UMETA(DisplayName = "Fist Weapon"),
	Dagger			UMETA(DisplayName = "Dagger"),
	Sword			UMETA(DisplayName = "Sword"),
	Axe				UMETA(DisplayName = "Axe"),
	Mace			UMETA(DisplayName = "Mace"),
	Staff			UMETA(DisplayName = "Staff"),
	Polearm			UMETA(DisplayName = "Polearm"),
	Bow				UMETA(DisplayName = "Bow"),
	Crossbow		UMETA(DisplayName = "Crossbow"),
	Shield			UMETA(DisplayName = "Shield"),

	NUM_WEAPON_TYPES
};

/** Represents a specific socket on a character to place an individual weapon */
UENUM(BlueprintType)
enum class EWeaponSocketType : uint8
{
	MainHandWeaponLoadoutOne		UMETA(DisplayName = "Main Hand Weapon Loadout One"),
	OffHandWeaponLoadoutOne			UMETA(DisplayName = "Off Hand Weapon Loadout One"),
	OffHandShieldLoadoutOne			UMETA(DisplayName = "Off Hand Shield Loadout One"),
	TwoHandWeaponLoadoutOne			UMETA(DisplayName = "Two Hand Weapon Loadout One"),
	MainHandWeaponLoadoutTwo		UMETA(DisplayName = "Main Hand Weapon Loadout Two"),
	OffHandWeaponLoadoutTwo			UMETA(DisplayName = "Off Hand Weapon Loadout Two"),
	OffHandShieldLoadoutTwo			UMETA(DisplayName = "Off Hand Shield Loadout Two"),
	TwoHandWeaponLoadoutTwo			UMETA(DisplayName = "Two Hand Weapon Loadout Two"),
};

/** Represents a specific hand that is being requested to equip or unequip a weapon */
UENUM(BlueprintType)
enum class ERequestedHand : uint8
{
	MainHand	UMETA(DisplayName = "Main Hand"),
	OffHand		UMETA(DisplayName = "Off Hand"),
	Both		UMETA(DisplayName = "Both")
};

/** Represents a specific loadout type based on equipped weapons */
UENUM(BlueprintType)
enum class ELoadoutType : uint8
{
	Unarmed				UMETA(DisplayName = "Unarmed"),
	OneHand				UMETA(DisplayName = "One Hand"),
	DualWield			UMETA(DisplayName = "Dual Wield"),
	OneHandAndShield	UMETA(DisplayName = "One Hand & Shield"),
	TwoHand				UMETA(DisplayName = "Two Hand")
};

/** Struct that stores the details of an individual weapon loadout */
USTRUCT(BlueprintType)
struct FWeaponLoadout
{
	GENERATED_BODY()

	/** The main hand weapon slot in the loadout. This weapon will determine if an off hand weapon can be used. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* MainHandWeapon;

	/** The off hand weapon loadout. This can only be used if MainHandWeapon is a OneHand weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* OffHandWeapon;

	FWeaponLoadout()
	{

	}

	FWeaponLoadout(AWeapon* MainHandWeaponPtr, AWeapon* OffHandWeaponPtr)
	{
		MainHandWeapon = MainHandWeaponPtr;
		OffHandWeapon = OffHandWeaponPtr;
	}
};

/** Blueprint library class for static equipment functions */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonEquipmentLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable , Category = "Equipment\|Weapons")
	static ELoadoutType GetLoadoutType(const FWeaponLoadout Loadout);
};