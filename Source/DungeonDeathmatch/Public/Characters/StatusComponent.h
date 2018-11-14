// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DungeonDeathmatch.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

protected:	
	UPROPERTY(Replicated)
	bool bIsAlive;

	// The current value of the actor's health
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Vitals")
	float Health;

	// The base value of the actor's health when at full health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float BaseHealth;

	// The current value of the actor's mana
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Vitals")
	float Mana;

	// The base value of the actor's mana when at full mana
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float BaseMana;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Vitals")
	bool bCanRegenMana;

	// The base value of the actor's mana regen per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float BaseManaRegenRate;

	// The current value of the actor's stamina
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Vitals")
	float Stamina;

	// The base value of the actor's stamina when at full stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float BaseStamina;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Vitals")
	bool bCanRegenStamina;

	// The base value of the actor's stamina regen per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vitals")
	float BaseStaminaRegenRate;

public:
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Tick(float DeltaTime);

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpendMana(float ManaAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanRegenMana(bool CanRegen);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpendStamina(float StaminaAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanRegenStamina(bool CanRegen);

public:
	bool IsAlive() const;

	float GetHealth() const;

	float GetBaseHealth() const;

	float GetMana() const;

	float GetBaseMana() const;

	UFUNCTION(BlueprintCallable, Category = "Vitals")
	bool SpendMana(float ManaAmount);

	UFUNCTION(BlueprintCallable, Category = "Vitals")
	void SetCanRegenMana(bool CanRegen);

	float GetStamina() const;

	float GetBaseStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Vitals")
	bool SpendStamina(float StaminaAmount);

	UFUNCTION(BlueprintCallable, Category = "Vitals")
	void SetCanRegenStamina(bool CanRegen);
	
};
