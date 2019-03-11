// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GMSMenuWidgetBase.generated.h"

class IGMSMenuInterface;

/**
 * 
 */
UCLASS()
class GAMEMENUSYSTEM_API UGMSMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	IGMSMenuInterface* MenuInterface;

public:
	virtual void SetMenuInterface(IGMSMenuInterface* MenuInterface);

	virtual void Setup();

	virtual void Teardown();

protected:
	virtual bool Initialize() override;

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
