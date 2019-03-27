#pragma once

#include "CoreMinimal.h"

/**
 * Enum representation of all available mesh segments for a humanoid character
 */
UENUM(BlueprintType)
enum class EHumanoidMeshSegment : uint8
{
	Helm						UMETA(DisplayName = "Helm"),
	Hair						UMETA(DisplayName = "Hair"),
	Head						UMETA(DisplayName = "Head"),
	LeftShoulder				UMETA(DisplayName = "Left Shoulder"),
	RightShoulder				UMETA(DisplayName = "Right Shoulder"),
	Torso						UMETA(DisplayName = "Torso"),
	ChestArmor					UMETA(DisplayName = "ChestArmor"),
	LeftHand					UMETA(DisplayName = "LeftHand"),
	RightHand					UMETA(DisplayName = "RightHand"),
	Waist						UMETA(DisplayName = "Waist"),
	Legs						UMETA(DisplayName = "Legs"),
	LegArmor					UMETA(DisplayName = "LegArmor"),
	LeftFoot					UMETA(DisplayName = "LeftFoot"),
	RightFoot					UMETA(DisplayName = "RightFoot")
};