// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Net/UnrealNetwork.h"

#define TRACE_INTERACTABLE	ECC_GameTraceChannel1
#define TRACE_ATTACK		ECC_GameTraceChannel2

#define PHYSICAL_SURFACE_METAL		SurfaceType1
#define PHYSICAL_SURFACE_STONE		SurfaceType2
#define PHYSICAL_SURFACE_WOOD		SurfaceType3
#define PHYSICAL_SURFACE_LEATHER	SurfaceType4
#define PHYSICAL_SURFACE_CLOTH		SurfaceType5
#define PHYSICAL_SURFACE_FLESH		SurfaceType6

/* Stencil index mapping for outline post processing */
#define STENCIL_ITEM_DEFAULT	255
#define STENCIL_ITEM_UNCOMMON	254
#define STENCIL_ITEM_RARE		253
#define STENCIL_ITEM_EPIC		252
#define STENCIL_ITEM_LEGENDARY	251

template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameByValue((int64)Value).ToString();
}