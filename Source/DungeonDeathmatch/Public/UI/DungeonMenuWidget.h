// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonMenuWidget.generated.h"

class IMenuInterface;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	IMenuInterface* MenuInterface;

public:
	void SetMenuInterface(IMenuInterface* MenuInterface);

	void Setup();

	void Teardown();

protected:
	virtual bool Initialize() override;

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
