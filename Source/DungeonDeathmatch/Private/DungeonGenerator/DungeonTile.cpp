// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonTile.h"

// Sets default values
ADungeonTile::ADungeonTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADungeonTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

