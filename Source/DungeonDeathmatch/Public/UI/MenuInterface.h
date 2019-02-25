// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUNGEONDEATHMATCH_API IMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HostGame() = 0;

	virtual void JoinGame(uint32 Index) = 0;

	virtual void ExitToMainMenu() = 0;

	virtual void ExitToDesktop() = 0;

	virtual void RefreshServerList() = 0;
};
