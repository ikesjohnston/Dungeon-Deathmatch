// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Spell.generated.h"

class UParticleSystem;

UENUM(BlueprintType)
enum class ESpellCastType : uint8
{
	Instant		UMETA(DisplayName = "Instant"),
	Timed		UMETA(DisplayName = "Timed"),
	Channeled	UMETA(DeisplayName = "Channeled")
};

UCLASS()
class DUNGEONDEATHMATCH_API ASpell : public AWeapon
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	ESpellCastType CastType;

	// Mana cost for a single cast, or mana cost per second for channeled cast spells
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float ManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	UParticleSystem* CastParticles;

	// Time to cast (in seconds) if a timed cast spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float CastTime;

	// Time between ticks if a channeled cast spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float ChannelTickTime;

private:

	FTimerHandle TimerHandle_Cast;

	FTimerHandle TimerHandle_ChannelTick;

public:	
	// Sets default values for this actor's properties
	ASpell();
	virtual ~ASpell() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBeginPrimaryCast();

	UFUNCTION(BlueprintNativeEvent)
	void BeginPrimaryCast();

	virtual void NativeOnEndPrimaryFunction(ADungeonCharacter* FunctioningCharacter) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEndPrimaryCast();

	UFUNCTION(BlueprintNativeEvent)
	void EndPrimaryCast();

	virtual void NativeOnStartSecondaryFunction(ADungeonCharacter* FunctioningCharacter) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBeginSecondaryCast();

	UFUNCTION(BlueprintNativeEvent)
	void BeginSecondaryCast();

	virtual void NativeOnEndSecondaryFunction(ADungeonCharacter* FunctioningCharacter) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEndSecondaryCast();

	UFUNCTION(BlueprintNativeEvent)
	void EndSecondaryCast();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
