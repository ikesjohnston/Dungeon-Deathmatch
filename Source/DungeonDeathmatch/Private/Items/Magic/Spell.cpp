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

void ASpell::NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter)
{	
	if (Role == ROLE_Authority)
	{
		UStatusComponent* StatusComponent = Cast<UStatusComponent>(FunctioningCharacter->GetComponentByClass(UStatusComponent::StaticClass()));
		if (StatusComponent)
		{
			if (StatusComponent->GetMana() >= ManaCost)
			{
				StatusComponent->SpendMana(ManaCost);
				Super::NativeOnStartPrimaryFunction(FunctioningCharacter);
			}
		}
		Multicast_OnBeginPrimaryCast();
	}
}

void ASpell::Multicast_OnBeginPrimaryCast_Implementation()
{
	BeginPrimaryCast();
}

void ASpell::BeginPrimaryCast_Implementation()
{

}

void ASpell::NativeOnEndPrimaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	if (Role == ROLE_Authority)
	{
		UStatusComponent* StatusComponent = Cast<UStatusComponent>(FunctioningCharacter->GetComponentByClass(UStatusComponent::StaticClass()));
		if (StatusComponent)
		{
			if (StatusComponent->GetMana() >= ManaCost)
			{
				StatusComponent->SpendMana(ManaCost);
				Super::NativeOnEndPrimaryFunction(FunctioningCharacter);
			}
		}
		Multicast_OnEndPrimaryCast();
	}
}

void ASpell::Multicast_OnEndPrimaryCast_Implementation()
{
	EndPrimaryCast();
}

void ASpell::EndPrimaryCast_Implementation()
{

}

void ASpell::NativeOnStartSecondaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	if (Role == ROLE_Authority)
	{
		UStatusComponent* StatusComponent = Cast<UStatusComponent>(FunctioningCharacter->GetComponentByClass(UStatusComponent::StaticClass()));
		if (StatusComponent)
		{
			if (StatusComponent->GetMana() >= ManaCost)
			{
				StatusComponent->SpendMana(ManaCost);
				Super::NativeOnStartSecondaryFunction(FunctioningCharacter);
			}
		}
		Multicast_OnBeginSecondaryCast();
	}
}

void ASpell::Multicast_OnBeginSecondaryCast_Implementation()
{
	BeginSecondaryCast();
}

void ASpell::BeginSecondaryCast_Implementation()
{

}

void ASpell::NativeOnEndSecondaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	if (Role == ROLE_Authority)
	{
		UStatusComponent* StatusComponent = Cast<UStatusComponent>(FunctioningCharacter->GetComponentByClass(UStatusComponent::StaticClass()));
		if (StatusComponent)
		{
			if (StatusComponent->GetMana() >= ManaCost)
			{
				StatusComponent->SpendMana(ManaCost);
				Super::NativeOnEndSecondaryFunction(FunctioningCharacter);
			}
		}
		Multicast_OnEndSecondaryCast();
	}
}

void ASpell::Multicast_OnEndSecondaryCast_Implementation()
{
	EndSecondaryCast();
}


void ASpell::EndSecondaryCast_Implementation()
{

}

