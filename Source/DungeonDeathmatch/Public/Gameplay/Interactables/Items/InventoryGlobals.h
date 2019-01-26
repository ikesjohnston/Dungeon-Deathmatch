#pragma once

#include "CoreMinimal.h"
#include "InventoryGlobals.generated.h"

/** Struct that stores an inventory grid row column pair */
USTRUCT(BlueprintType)
struct FInventoryGridPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Column;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Row;

	FInventoryGridPair()
	{
		Row = 0;
		Column = 0;
	}

	FInventoryGridPair(uint8 GridColumn, uint8 GridRow)
	{
		Column = GridColumn;
		Row = GridRow;
	}
};