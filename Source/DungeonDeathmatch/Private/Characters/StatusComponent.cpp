// Fill out your copyright notice in the Description page of Project Settings.

#include "StatusComponent.h"
#include <UnrealMathUtility.h>
#include "Game/DungeonGameMode.h"
#include <Components/ActorComponent.h>


// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// May change this to use timers if it takes up too much resources
	PrimaryComponentTick.bCanEverTick = true;

	bIsAlive = true;

	BaseHealth = 100;

	BaseMana = 100;
	bCanRegenMana = true;
	BaseManaRegenRate = 5;

	BaseStamina = 100;
	bCanRegenStamina = true;
	BaseStaminaRegenRate = 20;

	SetIsReplicated(true);
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, bIsAlive);
	DOREPLIFETIME(UStatusComponent, Health);
	DOREPLIFETIME(UStatusComponent, Mana);
	DOREPLIFETIME(UStatusComponent, bCanRegenMana);
	DOREPLIFETIME(UStatusComponent, Stamina);
	DOREPLIFETIME(UStatusComponent, bCanRegenStamina);
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UStatusComponent::HandleTakeAnyDamage);
		}
	}

	Health = BaseHealth;
	Mana = BaseMana;
	Stamina = BaseStamina;
}

void UStatusComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Server_Tick(DeltaTime);
}

void UStatusComponent::Server_Tick_Implementation(float DeltaTime)
{
	if (bCanRegenMana && Mana < BaseMana)
	{
		Mana += (BaseManaRegenRate * DeltaTime);
	}
	if (bCanRegenStamina && Stamina < BaseStamina)
	{
		Stamina += (BaseStaminaRegenRate * DeltaTime);
	}
}

bool UStatusComponent::Server_Tick_Validate(float DeltaTime)
{
	return true;
}

bool UStatusComponent::IsAlive() const
{
	return bIsAlive;
}

float UStatusComponent::GetHealth() const
{
	return Health;
}

float UStatusComponent::GetBaseHealth() const
{
	return BaseHealth;
}

void UStatusComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	Server_HandleTakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UStatusComponent::Server_HandleTakeAnyDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || !bIsAlive)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, BaseHealth);

	bIsAlive = Health >= 0.0f;

	if (Health <= 0)
	{
		ADungeonGameMode* GameMode = Cast<ADungeonGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

bool UStatusComponent::Server_HandleTakeAnyDamage_Validate(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	return true;
}

float UStatusComponent::GetMana() const
{
	return Mana;
}

float UStatusComponent::GetBaseMana() const
{
	return BaseMana;
}

bool UStatusComponent::SpendMana(float ManaAmount)
{
	if (Mana < ManaAmount)
		return false;
	Server_SpendMana(ManaAmount);
	return true;
}

void UStatusComponent::Server_SpendMana_Implementation(float ManaAmount)
{
	Mana = FMath::Clamp(Mana - ManaAmount, 0.0f, BaseMana);
}

bool UStatusComponent::Server_SpendMana_Validate(float ManaAmount)
{
	return true;
}

void UStatusComponent::SetCanRegenMana(bool CanRegen)
{
	Server_SetCanRegenMana(CanRegen);
}

void UStatusComponent::Server_SetCanRegenMana_Implementation(bool CanRegen)
{
	bCanRegenMana = CanRegen;
}

bool UStatusComponent::Server_SetCanRegenMana_Validate(bool CanRegen)
{
	return true;
}

float UStatusComponent::GetStamina() const
{
	return Stamina;
}

float UStatusComponent::GetBaseStamina() const
{
	return BaseStamina;
}

bool UStatusComponent::SpendStamina(float StaminaAmount)
{
	if (Stamina < StaminaAmount)
		return false;
	Server_SpendStamina(StaminaAmount);
	return true;
}

void UStatusComponent::Server_SpendStamina_Implementation(float StaminaAmount)
{
	Stamina = FMath::Clamp(Stamina - StaminaAmount, 0.0f, BaseStamina);
}

bool UStatusComponent::Server_SpendStamina_Validate(float StaminaAmount)
{
	return true;
}

void UStatusComponent::SetCanRegenStamina(bool CanRegen)
{
	Server_SetCanRegenStamina(CanRegen);
}

void UStatusComponent::Server_SetCanRegenStamina_Implementation(bool CanRegen)
{
	bCanRegenStamina = CanRegen;
}

bool UStatusComponent::Server_SetCanRegenStamina_Validate(bool CanRegen)
{
	return true;
}
