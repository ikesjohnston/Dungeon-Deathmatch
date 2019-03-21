// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DungeonEnums.h"
#include "DungeonGenerator.generated.h"

class ADungeonTile;

UCLASS()
class DUNGEONDEATHMATCH_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon")
	FIntVector RoomDungeonGridSize;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon")
	FVector DungeonTileSize;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon", meta = (ClampMin = 1))
	int32 NumberOfRooms;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon")
	FIntVector RoomSizeMin;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon")
	FIntVector RoomSizeMax;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon", meta = (ClampMin = 1))
	int32 MinRoomDistance;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon", meta = (ClampMin = 1.0f))
	float MainRoomSizeFactor;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon", meta = (ClampMin = 1))
	int32 MaxPlacementAttempts;

	UPROPERTY(VisibleAnywhere, Category = "Room Style Dungeon")
	TArray<FDungeonRoom> Rooms;

	TArray<FDungeonConnection> Connections;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon")
	bool bAddExtraConnections;

	UPROPERTY(EditAnywhere, Category = "Room Style Dungeon", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AdditionalConnectionsRatio;

	TMap<FIntVector, bool> RoomDungeonGrid;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	FIntVector DungeonGridSize;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	FVector TileSize;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon", meta = (ClampMin = 1))
	int32 NumberOfTiles;

	TMap<FIntVector, FTileData> Tiles;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	TSubclassOf<ADungeonTile> DeadEndTileClass;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	TSubclassOf<ADungeonTile> CorridorTileClass;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	TSubclassOf<ADungeonTile> CornerTileClass;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	TSubclassOf<ADungeonTile> ThreeWayTileClass;

	UPROPERTY(EditAnywhere, Category = "Tile Style Dungeon")
	TSubclassOf<ADungeonTile> FourWayTileClass;

private:
	FIntVector RoomSizeTotal;
	FIntVector RoomSizeAverage;

public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dungeon")
	void GenerateDungeon();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dungeon")
	void GenerateRoomBasedDungeon();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dungeon")
	void DestroyDungeon();

protected:
	TSubclassOf<ADungeonTile> GetTileClass(ETileType Type);

	void GenerateRoom();

	void BuildConnections();

	void DrawDebugDungeon();

	void GenerateTileLayout();

	void SpawnTileLayout();

	FVector GetRandomPointInSphere(float Radius);
};
