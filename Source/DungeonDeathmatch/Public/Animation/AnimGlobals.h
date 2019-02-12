// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/** Used for animation blueprints to track if animations are playing for a particular slot. */
UENUM(BlueprintType)
enum class EAnimSlot : uint8
{
	Default		UMETA(DisplayName = "Default"),
	UpperBody	UMETA(DisplayName = "Upper Body"),
	MainHand	UMETA(DisplayName = "Main Hand"),
	OffHand		UMETA(DisplayName = "OffHand")
};