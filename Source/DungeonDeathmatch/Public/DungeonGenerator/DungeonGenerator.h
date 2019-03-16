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
	UPROPERTY(EditAnywhere)
	FIntVector DungeonGridSize;

	UPROPERTY(EditAnywhere)
	FVector TileSize;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
	int32 NumberOfTiles;

	TMap<FIntVector, FTileData> Tiles;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADungeonTile> DeadEndTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADungeonTile> CorridorTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADungeonTile> CornerTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADungeonTile> ThreeWayTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADungeonTile> FourWayTileClass;

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
	void DestroyDungeon();

protected:
	TSubclassOf<ADungeonTile> GetTileClass(ETileType Type);

	void GenerateTileLayout();

	void SpawnTileLayout();
};
