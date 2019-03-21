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

USTRUCT()
struct FDungeonRoom
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid GUID;

	UPROPERTY(VisibleAnywhere)
	FIntVector Location;

	UPROPERTY(VisibleAnywhere)
	FIntVector Size;

	FDungeonRoom(FIntVector Location = FIntVector(0, 0, 0), FIntVector Size = FIntVector(1, 1, 1))
	{
		GUID = FGuid::NewGuid();

		this->Location = Location;
		this->Size = Size;
	}

	bool operator==(const FDungeonRoom& Other) const
	{
		return GUID == Other.GUID;
	}

	bool operator!=(const FDungeonRoom& Other) const
	{
		return GUID != Other.GUID;
	}

	friend uint32 GetTypeHash(const FDungeonRoom& Other)
	{
		return GetTypeHash(Other.GUID);
	}
};

USTRUCT()
struct FDungeonConnection
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FDungeonRoom RoomOne;

	UPROPERTY()
	FDungeonRoom RoomTwo;

	float Weight;

public:
	FDungeonConnection()
	{
		RoomOne = FDungeonRoom();
		RoomTwo = FDungeonRoom();

		CalculateWeight();
	}

	FDungeonConnection(FDungeonRoom& InFromRoom, FDungeonRoom& InToRoom)
	{
		RoomOne = InFromRoom;
		RoomTwo = InToRoom;

		CalculateWeight();
	}

	FDungeonRoom GetRoomOne() const
	{
		return RoomOne;
	}

	void SetRoomOne(FDungeonRoom& Room)
	{
		RoomOne = Room;
		CalculateWeight();
	}

	FDungeonRoom GetRoomTwo() const
	{
		return RoomTwo;
	}

	void SetRoomTwo(FDungeonRoom& Room)
	{
		RoomTwo = Room;
		CalculateWeight();
	}

	float GetWeight() const
	{
		return Weight;
	}
	
	bool Contains(FDungeonRoom& Room) const
	{
		return (Room == RoomOne || Room == RoomTwo);
	}

	bool operator==(const FDungeonConnection& Other) const
	{
		// Connections are undirected
		if ((RoomOne == Other.GetRoomOne() && RoomTwo == Other.GetRoomTwo()) || (RoomOne == Other.GetRoomTwo() && RoomTwo == Other.GetRoomOne()))
		{
			return true;
		}
		return false;
	}

	bool operator!=(const FDungeonConnection& Other) const
	{
		return !operator==(Other);
	}

private:
	void CalculateWeight()
	{
		FVector RoomOneLocation = FVector(RoomOne.Location.X, RoomOne.Location.Y, RoomOne.Location.Z);
		FVector RoomTwoLocation = FVector(RoomTwo.Location.X, RoomTwo.Location.Y, RoomTwo.Location.Z);
		Weight = FVector::Distance(RoomOneLocation, RoomTwoLocation);
	}
};

struct FDungeonConnectionPredicate
{
	bool operator()(const FDungeonConnection& A, const FDungeonConnection& B) const
	{
		// Inverted compared to std::priority_queue - higher priorities float to the top
		return A.GetWeight() < B.GetWeight();
	}
};

USTRUCT()
struct FDungeonConnectionSet
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FDungeonRoom> Set;

	FDungeonConnectionSet()
	{

	}

	bool operator==(const FDungeonConnectionSet& Other) const
	{
		if (Set.Num() != Other.Set.Num())
		{
			return false;
		}

		for (FDungeonRoom Room : Set)
		{
			if (!Other.Set.Contains(Room)) 
			{
				return false;
			}
		}

		return true;
	}

	FDungeonConnectionSet Union(const FDungeonConnectionSet OtherSet) const
	{
		FDungeonConnectionSet UnionedSet;
		UnionedSet.Set = Set;
		for (FDungeonRoom Room : OtherSet.Set)
		{
			if (!UnionedSet.Set.Contains(Room))
			{
				UnionedSet.Set.Add(Room);
			}
		}
		return UnionedSet;
	}
};