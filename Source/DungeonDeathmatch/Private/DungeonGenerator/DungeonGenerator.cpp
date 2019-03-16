// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator.h"
#include <DrawDebugHelpers.h>
#include "DungeonGenerator/DungeonTile.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateDungeon();
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

