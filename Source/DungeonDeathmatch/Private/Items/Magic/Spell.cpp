// Fill out your copyright notice in the Description page of Project Settings.

#include "Spell.h"
#include "DungeonCharacter.h"
#include "StatusComponent.h"


// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManaCost = 30.0f;
	CastTime = 1.0f;
	ChannelTickTime = 1.0f;
}

ASpell::~ASpell()
{

}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpell::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{	

}
