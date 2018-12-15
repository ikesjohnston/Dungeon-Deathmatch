// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGameInstance.h"

UDungeonGameInstance::UDungeonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FStreamableManager& UDungeonGameInstance::GetAssetLoader()
{
	return AssetLoader;
}
