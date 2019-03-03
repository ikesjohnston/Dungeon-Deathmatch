#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerInput.h>
#include "InputGlobals.generated.h"

/**
 * Struct for storing primary and secondary key binding strings
 */
USTRUCT()
struct FDungeonKeyBind
{
	GENERATED_BODY()

	FString ActionName;
	FInputActionKeyMapping PrimaryBinding;
	FInputActionKeyMapping SecondaryBinding;
};