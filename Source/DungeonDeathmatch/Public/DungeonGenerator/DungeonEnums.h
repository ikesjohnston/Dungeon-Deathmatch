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

		TileActor = nullptr;
	}

	bool operator==(const FTileData& Other) const
	{
		return GUID == Other.GUID;
	}

	ETileConnection GetConnection()
	{
		bool ConnectedNorth = Directions[ECardinalDirection::North];
		bool ConnectedSouth = Directions[ECardinalDirection::South];
		bool ConnectedEast = Directions[ECardinalDirection::East];
		bool ConnectedWest = Directions[ECardinalDirection::West];

		if (ConnectedNorth && ConnectedSouth && ConnectedEast && ConnectedWest)
		{
			return ETileConnection::NSEW;
		}
		else if (ConnectedNorth && ConnectedSouth && ConnectedEast)
		{
			return ETileConnection::NSE;
		}
		else if (ConnectedNorth && ConnectedSouth && ConnectedWest)
		{
			return ETileConnection::NSW;
		}
		else if (ConnectedNorth && ConnectedEast && ConnectedWest)
		{
			return ETileConnection::NEW;
		}
		else if (ConnectedSouth && ConnectedEast && ConnectedWest)
		{
			return ETileConnection::SEW;
		}
		else if (ConnectedNorth && ConnectedSouth)
		{
			return ETileConnection::NS;
		}
		else if (ConnectedNorth && ConnectedEast)
		{
			return ETileConnection::NE;
		}
		else if (ConnectedNorth && ConnectedWest)
		{
			return ETileConnection::NW;
		}
		else if (ConnectedEast && ConnectedWest)
		{
		return ETileConnection::EW;
		}
		else if (ConnectedSouth && ConnectedEast)
		{
			return ETileConnection::SE;
		}
		else if (ConnectedSouth && ConnectedWest)
		{
			return ETileConnection::SW;
		}
		else if (ConnectedNorth)
		{
			return ETileConnection::N;
		}
		else if (ConnectedSouth)
		{
			return ETileConnection::S;
		}
		else if (ConnectedEast)
		{
			return ETileConnection::E;
		}
		else
		{
			return ETileConnection::W;
		}
	}
};