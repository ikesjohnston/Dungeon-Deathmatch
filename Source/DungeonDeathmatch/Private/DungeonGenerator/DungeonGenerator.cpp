// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator.h"
#include "DungeonGenerator/DungeonTile.h"

#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoomDungeonGridSize = FIntVector(50, 50, 10);
	NumberOfRooms = 50;
	DungeonTileSize = FVector(500.0f, 500.0f, 500.0f);
	RoomSizeMin = FIntVector(3, 3, 2);
	RoomSizeMax = FIntVector(10, 10, 4);
	MinRoomDistance = 5;
	MainRoomSizeFactor = 1.2f;
	MaxPlacementAttempts = 1000;

	bAddExtraConnections = true;
	AdditionalConnectionsRatio = 0.25f;
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADungeonGenerator::GenerateDungeon()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Dungeon!"));
	DestroyDungeon();
	GenerateTileLayout();
	SpawnTileLayout();
}

void ADungeonGenerator::GenerateRoomBasedDungeon()
{
	FDateTime GenerationStartTime = FDateTime::Now();
	UE_LOG(LogTemp, Warning, TEXT("Generating Dungeon!"));

	GenerateRooms();
	BuildConnections();

	FDateTime GenerationEndTime = FDateTime::Now();
	UE_LOG(LogTemp, Warning, TEXT("Generated %d rooms with %d connections in %f seconds"), Rooms.Num(), Connections.Num(), (GenerationEndTime - GenerationStartTime).GetTotalSeconds());

	DrawDebugDungeon();
}

void ADungeonGenerator::GenerateRooms()
{
	Rooms.Empty();
	RoomDungeonGrid.Empty();
	RoomSizeTotal = FIntVector(0, 0, 0);
	if (RoomSizeMax.Size() > DungeonGridSize.Size())
	{
		UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateRoomBasedDungeon - Max room size exceeds dungeon grid size, aborting"));
		return;
	}
	for (int RoomIndex = 0; RoomIndex < NumberOfRooms; RoomIndex++)
	{
		GenerateRoom();
	}
	RoomSizeAverage = RoomSizeTotal / NumberOfRooms;
}

void ADungeonGenerator::GenerateRoom()
{
	FIntVector RoomSize = FIntVector(FMath::RandRange(RoomSizeMin.X, RoomSizeMax.X), FMath::RandRange(RoomSizeMin.Y, RoomSizeMax.Y), FMath::RandRange(RoomSizeMin.Z, RoomSizeMax.Z));
	RoomSizeTotal += RoomSize;
	FIntVector RoomLocation;
	bool LocationValid = false;
	int PlacementAttempts = 0;
	while (!LocationValid && PlacementAttempts < MaxPlacementAttempts)
	{
		LocationValid = true;
		FVector MaxRoomPoint = FVector(RoomDungeonGridSize.X - RoomSize.X, RoomDungeonGridSize.Y - RoomSize.Y, RoomDungeonGridSize.Z - RoomSize.Z);
		FVector RandomDungeonLocation = FMath::RandPointInBox(FBox(-MaxRoomPoint, MaxRoomPoint));
		RoomLocation = FIntVector(RandomDungeonLocation.X, RandomDungeonLocation.Y, RandomDungeonLocation.Z);

		int RoomXMin = RoomLocation.X - MinRoomDistance;
		int RoomXMax = RoomLocation.X + RoomSize.X + MinRoomDistance;

		int RoomYMin = RoomLocation.Y - MinRoomDistance;
		int RoomYMax = RoomLocation.Y + RoomSize.Y + MinRoomDistance;

		int RoomZMin = RoomLocation.Z - MinRoomDistance;
		int RoomZMax = RoomLocation.Z + RoomSize.Z + MinRoomDistance;

		for (int RoomX = RoomXMin; RoomX < RoomXMax; RoomX++)
		{
			if (!LocationValid) break;
			for (int RoomY = RoomYMin; RoomY < RoomYMax; RoomY++)
			{
				if (!LocationValid) break;
				for (int RoomZ = RoomZMin; RoomZ < RoomZMax; RoomZ++)
				{
					FIntVector RoomGridCell = FIntVector(RoomX, RoomY, RoomZ);
					if (RoomDungeonGrid.Find(RoomGridCell))
					{
						LocationValid = false;
						break;
					}
				}
			}
		}
		PlacementAttempts++;
		if (PlacementAttempts >= MaxPlacementAttempts)
		{
			UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateRoom - Exceeded max room placement attempts, room placement is invalid"));
		}
	}

	FDungeonRoom Room = FDungeonRoom(RoomLocation, RoomSize);
	Rooms.Add(Room);

	int RoomXMin = RoomLocation.X;
	int RoomXMax = RoomLocation.X + RoomSize.X;

	int RoomYMin = RoomLocation.Y;
	int RoomYMax = RoomLocation.Y + RoomSize.Y;

	int RoomZMin = RoomLocation.Z;
	int RoomZMax = RoomLocation.Z + RoomSize.Z;

	for (int RoomX = RoomXMin; RoomX < RoomXMax; RoomX++)
	{
		for (int RoomY = RoomYMin; RoomY < RoomYMax; RoomY++)
		{
			for (int RoomZ = RoomZMin; RoomZ < RoomZMax; RoomZ++)
			{
				FIntVector RoomGridCell = FIntVector(RoomX, RoomY, RoomZ);
				RoomDungeonGrid.Add(TTuple<FIntVector, bool>(RoomGridCell, true));
			}
		}
	}
}

void ADungeonGenerator::BuildConnections()
{
	Connections.Empty();

	// Initially making connections from every room to every other room for now
	for (FDungeonRoom Room : Rooms)
	{
		for (FDungeonRoom OtherRoom : Rooms)
		{
			if (Room != OtherRoom)
			{
				FDungeonConnection Connection = FDungeonConnection(Room, OtherRoom);
				Connections.HeapPush(Connection, FDungeonConnectionPredicate());
			}
		}
	}

	// Create minimum spanning tree from connections
	TArray<FDungeonConnection> TempConnections = Connections;
	TArray<FDungeonConnection> DungeonConnections;
	TArray<FDungeonConnectionSet> ConnectionSets;
	for (int ConnectionIndex = 0; ConnectionIndex < Connections.Num(); ConnectionIndex++)
	{
		FDungeonConnection NextConnection;
		TempConnections.HeapPop(NextConnection, FDungeonConnectionPredicate());

		bool CycleDetected = false;
		FDungeonConnectionSet RoomOneSet;
		RoomOneSet.Set.Add(NextConnection.GetRoomOne());
		FDungeonConnectionSet RoomTwoSet;
		RoomTwoSet.Set.Add(NextConnection.GetRoomTwo());

		for (FDungeonConnectionSet ConnectionSet : ConnectionSets)
		{
			if (ConnectionSet.Set.Contains(NextConnection.GetRoomOne()))
			{
				RoomOneSet = ConnectionSet;
			}
			if (ConnectionSet.Set.Contains(NextConnection.GetRoomTwo()))
			{
				RoomTwoSet = ConnectionSet;
			}
			if (RoomOneSet == RoomTwoSet)
			{
				CycleDetected = true;
				break;
			}
		}

		if (!CycleDetected)
		{
			ConnectionSets.Remove(RoomOneSet);
			ConnectionSets.Remove(RoomTwoSet);
			FDungeonConnectionSet UnionedSet = RoomOneSet.Union(RoomTwoSet);
			ConnectionSets.Add(UnionedSet);

			DungeonConnections.Add(NextConnection);
		}

		if (DungeonConnections.Num() == Rooms.Num() - 1)
		{
			break;
		}
	}

	// Add a percentage of connections back (based on room total for now)
	if (bAddExtraConnections)
	{
		int AdditionalConnections = NumberOfRooms * AdditionalConnectionsRatio;

		for (int ConnectionIndex = 0; ConnectionIndex < AdditionalConnections; ConnectionIndex++)
		{
			// Pick a random room and add the shortest edge for that room not already connected
			FDungeonRoom& Room = Rooms[FMath::RandRange(0, Rooms.Num() - 1)];
			FDungeonConnection NextConnection;
			bool ConnectionAlreadyMade = true;
			while (ConnectionAlreadyMade)
			{
				TempConnections.HeapPop(NextConnection, FDungeonConnectionPredicate());
				if (NextConnection.Contains(Room))
				{
					ConnectionAlreadyMade = DungeonConnections.Contains(NextConnection);
				}
			}
			DungeonConnections.Add(NextConnection);
		}
	}

	Connections = DungeonConnections;
}

void ADungeonGenerator::DrawDebugDungeon()
{
	FlushPersistentDebugLines(GetWorld());
	for (FDungeonRoom Room : Rooms)
	{
		FColor DebugColor = FColor::Blue;
		if (Room.Size.Size() > (RoomSizeAverage.Size() * MainRoomSizeFactor))
		{
			DebugColor = FColor::Red;
		}
		FVector RoomCenter = FVector(Room.Location.X * DungeonTileSize.X, Room.Location.Y * DungeonTileSize.Y, Room.Location.Z * DungeonTileSize.Z);
		FVector RoomExtent = FVector((Room.Size.X * DungeonTileSize.X) / 2, (Room.Size.Y * DungeonTileSize.Y) / 2, (Room.Size.Z * DungeonTileSize.Z) / 2);
		DrawDebugBox(GetWorld(), RoomCenter, RoomExtent, FRotator::ZeroRotator.Quaternion(), DebugColor, false, 10000.0f, 0, 25.0f);
	}
	for (FDungeonConnection Connection : Connections)
	{
		FVector FromRoomLocation = FVector(Connection.GetRoomOne().Location.X * DungeonTileSize.X, Connection.GetRoomOne().Location.Y * DungeonTileSize.Y, Connection.GetRoomOne().Location.Z * DungeonTileSize.Z);
		FVector ToRoomLocation = FVector(Connection.GetRoomTwo().Location.X * DungeonTileSize.X, Connection.GetRoomTwo().Location.Y * DungeonTileSize.Y, Connection.GetRoomTwo().Location.Z * DungeonTileSize.Z);
		DrawDebugLine(GetWorld(), FromRoomLocation, ToRoomLocation, FColor::Orange, false, 10000.0f, 0, 25.0f);
	}
}

void ADungeonGenerator::DestroyDungeon()
{
	FlushPersistentDebugLines(GetWorld());

	for (TTuple<FIntVector, FTileData>& Tile : Tiles)
	{
		ADungeonTile* TileActor = Tile.Value.TileActor;
		if (TileActor)
		{
			GetWorld()->DestroyActor(TileActor);
		}
	}

	Tiles.Empty();
}

TSubclassOf<ADungeonTile> ADungeonGenerator::GetTileClass(ETileType Type)
{
	switch (Type)
	{
	case ETileType::Generic:
		break;
	case ETileType::Additional:
		break;
	case ETileType::Replacement:
		break;
	default:
		break;
	}

	return ADungeonTile::StaticClass();
}

void ADungeonGenerator::GenerateTileLayout()
{
	int32 TilesGenerated = 0;
	FTileData OriginTile = FTileData();
	FIntVector OriginTileLocation = FIntVector(DungeonGridSize.X / 2, DungeonGridSize.Y / 2, DungeonGridSize.Z / 2);
	Tiles.Add(TTuple<FIntVector, FTileData>(OriginTileLocation, OriginTile));
	TQueue<FTileData> TileQueue;
	TileQueue.Enqueue(OriginTile);
	TilesGenerated++;

	while (TilesGenerated < NumberOfTiles)
	{
		FTileData ActiveTile;
		TileQueue.Dequeue(ActiveTile);

		const FIntVector* pActiveTileLocation = Tiles.FindKey(ActiveTile);
		if (!pActiveTileLocation)
		{
			UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateTileLayout - Could not get active tile location"));
			continue;
		}
		TArray<ECardinalDirection> AvailableConnectionDirections;
		for (TTuple<ECardinalDirection, bool>& DirectionMapping : ActiveTile.Directions)
		{
			bool IsOutsideOfGrid = false;

			switch (DirectionMapping.Key)
			{
			case ECardinalDirection::North:
				DirectionMapping.Value = Tiles.Contains(*pActiveTileLocation + FIntVector(1, 0, 0));
				IsOutsideOfGrid = (pActiveTileLocation->X + 1 >= DungeonGridSize.X);
				break;
			case ECardinalDirection::South:
				DirectionMapping.Value = Tiles.Contains(*pActiveTileLocation + FIntVector(-1, 0, 0));
				IsOutsideOfGrid = (pActiveTileLocation->X - 1 < 0);
				break;
			case ECardinalDirection::East:
				DirectionMapping.Value = Tiles.Contains(*pActiveTileLocation + FIntVector(0, 1, 0));
				IsOutsideOfGrid = (pActiveTileLocation->Y + 1 >= DungeonGridSize.Y);
				break;
			case ECardinalDirection::West:
				DirectionMapping.Value = Tiles.Contains(*pActiveTileLocation + FIntVector(0, -1, 0));
				IsOutsideOfGrid = (pActiveTileLocation->Y + 1 >= DungeonGridSize.Y);
				break;
			default:
				break;
			}

			// TODO: Should the direction mapping be updated or should we just continue?
			if (IsOutsideOfGrid || DirectionMapping.Value)
			{
				//UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateTileLayout - Tried to place tile at out of bounds location"));
				continue;
			}
			AvailableConnectionDirections.Add(DirectionMapping.Key);
		}

		if (AvailableConnectionDirections.Num() > 0)
		{
			ECardinalDirection NewTileDirection = AvailableConnectionDirections[FMath::RandRange(0, AvailableConnectionDirections.Num() - 1)];
			FTileData NewTile;
			ActiveTile.Directions[NewTileDirection] = true;

			FIntVector NewTileLocation = *pActiveTileLocation;

			switch (NewTileDirection)
			{
			case ECardinalDirection::North:
				NewTile.Directions[ECardinalDirection::South] = true;
				NewTileLocation.X++;
				break;
			case ECardinalDirection::South:
				NewTile.Directions[ECardinalDirection::North] = true;
				NewTileLocation.X--;
				break;
			case ECardinalDirection::East:
				NewTile.Directions[ECardinalDirection::West] = true;
				NewTileLocation.Y++;
				break;
			case ECardinalDirection::West:
				NewTile.Directions[ECardinalDirection::East] = true;
				NewTileLocation.Y--;
				break;
			default:
				break;
			}

			FVector SpawnOffset = FVector(TileSize.X * DungeonGridSize.X / 2, TileSize.Y * DungeonGridSize.Y / 2, TileSize.Z * DungeonGridSize.Z / 2);
			FVector ActiveLocationVector = FVector(pActiveTileLocation->X * TileSize.X, pActiveTileLocation->Y * TileSize.Y, pActiveTileLocation->Z * TileSize.Z) - SpawnOffset;
			FVector NewLocationVector = FVector(NewTileLocation.X * TileSize.X, NewTileLocation.Y * TileSize.Y, NewTileLocation.Z * TileSize.Z) - SpawnOffset;
			FVector DrawDebugOffset = FVector(0, 0, 100);

			if (Tiles.Find(NewTileLocation))
			{
				UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateTileLayout - Tile %d overwrote tile at (%d, %d, %d)"), TilesGenerated + 1, NewTileLocation.X, NewTileLocation.Y, NewTileLocation.Z);
				DrawDebugPoint(GetWorld(), NewLocationVector + DrawDebugOffset, 100.0, FColor::Red);
				return;
			}

			DrawDebugPoint(GetWorld(), NewLocationVector + DrawDebugOffset, 150.0, FColor::Cyan);
			DrawDebugLine(GetWorld(), ActiveLocationVector + DrawDebugOffset, NewLocationVector + DrawDebugOffset, FColor::Red, false, 10.0);

			Tiles.Add(TTuple<FIntVector, FTileData>(NewTileLocation, NewTile));
			const FIntVector* ActiveTileKey = Tiles.FindKey(ActiveTile);
			if (ActiveTileKey)
			{
				FIntVector TempKey = *ActiveTileKey;
				Tiles.Remove(*ActiveTileKey);
				Tiles.Add(TTuple<FIntVector, FTileData>(TempKey, ActiveTile));
			}

			//TODO: Check adjacent tiles and mark as connected?

			TileQueue.Enqueue(NewTile);
			TileQueue.Enqueue(ActiveTile);

			TilesGenerated++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateTileLayout - Failed to generate a connection for tile (%d, %d, %d)"), pActiveTileLocation->X, pActiveTileLocation->Y, pActiveTileLocation->Z);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("ADungeonGenerator::GenerateTileLayout - Generated %d tiles"), TilesGenerated);
}

void ADungeonGenerator::SpawnTileLayout()
{
	FVector SpawnOffset = FVector(TileSize.X * DungeonGridSize.X / 2, TileSize.Y * DungeonGridSize.Y / 2, TileSize.Z * DungeonGridSize.Z / 2);

	for (TTuple<FIntVector, FTileData>& Tile : Tiles)
	{
		FVector SpawnLocation = FVector(Tile.Key.X * TileSize.X, Tile.Key.Y * TileSize.Y, Tile.Key.Z * TileSize.Z) - SpawnOffset;
		FRotator SpawnRotation;
		TSubclassOf<ADungeonTile> TileClass = ADungeonTile::StaticClass();
		switch (Tile.Value.GetConnection())
		{
		case ETileConnection::N:
			TileClass = DeadEndTileClass;
			SpawnRotation = FRotator(0, 0, 0);
			break;
		case ETileConnection::S:
			TileClass = DeadEndTileClass;
			SpawnRotation = FRotator(0, 180, 0);
			break;
		case ETileConnection::E:
			TileClass = DeadEndTileClass;
			SpawnRotation = FRotator(0, 90, 0);
			break;
		case ETileConnection::W:
			TileClass = DeadEndTileClass;
			SpawnRotation = FRotator(0, -90, 0);
			break;
		case ETileConnection::NS:
			TileClass = CorridorTileClass;
			SpawnRotation = FRotator(0, 0, 0);
			break;
		case ETileConnection::NE:
			TileClass = CornerTileClass;
			SpawnRotation = FRotator(0, 0, 0);
			break;
		case ETileConnection::NW:
			TileClass = CornerTileClass;
			SpawnRotation = FRotator(0, -90, 0);
			break;
		case ETileConnection::SE:
			TileClass = CornerTileClass;
			SpawnRotation = FRotator(0, 90, 0);
			break;
		case ETileConnection::SW:
			TileClass = CornerTileClass;
			SpawnRotation = FRotator(0, 180, 0);
			break;
		case ETileConnection::EW:
			TileClass = CorridorTileClass;
			SpawnRotation = FRotator(0, 90, 0);
			break;
		case ETileConnection::NSE:
			TileClass = ThreeWayTileClass;
			SpawnRotation = FRotator(0, 90, 0);
			break;
		case ETileConnection::NSW:
			TileClass = ThreeWayTileClass;
			SpawnRotation = FRotator(0, -90, 0);
			break;
		case ETileConnection::SEW:
			TileClass = ThreeWayTileClass;
			SpawnRotation = FRotator(0, 180, 0);
			break;
		case ETileConnection::NEW:
			TileClass = ThreeWayTileClass;
			SpawnRotation = FRotator(0, 0, 0);
			break;
		case ETileConnection::NSEW:
			TileClass = FourWayTileClass;
			SpawnRotation = FRotator(0, 0, 0);
			break;
		default:
			break;
		}

		FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation, FVector::OneVector);
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Tile.Value.TileActor = GetWorld()->SpawnActor<ADungeonTile>(TileClass, SpawnTransform, SpawnParams);

		DrawDebugBox(GetWorld(), SpawnLocation, FVector(TileSize.X / 2, TileSize.Y / 2, TileSize.Z / 2), FRotator::ZeroRotator.Quaternion(), FColor::Green, false, 10.0f);
	}
}

FVector ADungeonGenerator::GetRandomPointInSphere(float Radius)
{
	return (FMath::VRand() * FMath::RandRange(0.0f, Radius));
}

