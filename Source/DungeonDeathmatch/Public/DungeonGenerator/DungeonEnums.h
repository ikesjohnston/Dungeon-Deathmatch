#pragma once

#include "CoreMinimal.h"

#include "DungeonEnums.generated.h"

class ADungeonTile;

UENUM(BlueprintType) enum class ECardinalDirection : uint8 {
	North		UMETA(DisplayName = "North"),
	South		UMETA(DisplayName = "South"),
	East		UMETA(DisplayName = "East"),
	West		UMETA(DisplayName = "West"),
};

UENUM(BlueprintType) enum class ETileConnection : uint8 {
	N		UMETA(DisplayName = "N"),
	S		UMETA(DisplayName = "S"),
	E		UMETA(DisplayName = "E"),
	W		UMETA(DisplayName = "W"),
	NS		UMETA(DisplayName = "NS"),
	NE		UMETA(DisplayName = "NE"),
	NW		UMETA(DisplayName = "NW"),
	SE		UMETA(DisplayName = "SE"),
	SW		UMETA(DisplayName = "SW"),
	EW		UMETA(DisplayName = "EW"),
	NSE		UMETA(DisplayName = "NSE"),
	NSW		UMETA(DisplayName = "NSW"),
	SEW		UMETA(DisplayName = "SEW"),
	NEW		UMETA(DisplayName = "NEW"),
	NSEW	UMETA(DisplayName = "NSEW")
};

UENUM(BlueprintType) enum class ETileType : uint8 {
	Generic			UMETA(DisplayName = "Standard"),
	Additional		UMETA(DisplayName = "Additional"),
	Replacement		UMETA(DisplayName = "Replacement")
};

USTRUCT()
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid GUID;

	UPROPERTY()
	ETileType Type;

	UPROPERTY()
	TMap<ECardinalDirection, bool> Directions;

	UPROPERTY()
	ADungeonTile* TileActor;

	FTileData(ETileType TileType = ETileType::Generic, bool ConnectedNorth = false, bool ConnectedSouth = false, bool ConnectedEast = false, bool ConnectedWest = false)
	{
		GUID = FGuid::NewGuid();

		Type = TileType;

		Directions.Add(ECardinalDirection::North, ConnectedNorth);
		Directions.Add(ECardinalDirection::South, ConnectedSouth);
		Directions.Add(ECardinalDirection::East, ConnectedEast);
		Directions.Add(ECardinalDirection::West, ConnectedWest);
	}

	bool operator==(const FTileData& Other) const
	{
		return GUID == Other.GUID;
	}
};