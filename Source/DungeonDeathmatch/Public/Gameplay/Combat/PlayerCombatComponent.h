// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Combat/CombatEnums.h"
#include "PlayerCombatComponent.generated.h"

class APlayerCharacter;
class AWeapon;
class UDungeonGameplayAbility;
class UAbilitySystemComponent;
class UEquipmentComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** The Weapon class to use for the main hand when no other weapon is equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<AWeapon> MainHandUnarmedWeaponClass;

	/** The Weapon to use for the main hand when no other weapon is equipped. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AWeapon* MainHandUnarmedWeapon;

	/** The Weapon class to use for the off hand when no other weapon is equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<AWeapon> OffHandUnarmedWeaponClass;

	/** The Weapon to use for the off hand when no other weapon is equipped. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AWeapon* OffHandUnarmedWeapon;

	UPROPERTY(VisibleAnywhere, Replicated)
	ECombatState CombatState;

	/** The active combo type, if any. Used for determining what melee attack abilities to use. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EMeleeComboType ActiveMeleeComboType;

	/** Running tally of the combo count for the currently active combo type, if any */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	uint8 ActiveMeleeComboCount;

	/** Flag to determine if character can begin a new melee combo attack */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsMeleeComboReady;

	/** The GameplayAbility to use when sheathing weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> SheatheWeaponsAbility;

	/** The GameplayAbility to use when unsheathing weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> UnsheatheWeaponsAbility;

	/** The GameplayAbility to use when switching weapon loadouts */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> SwitchWeaponLoadoutAbility;

private:
	UAbilitySystemComponent* AbilitySystemComponent;

	UEquipmentComponent* EquipmentComponent;

	/** Timer used to end a combo a defined amount of time after an attack ends */
	FTimerHandle MeleeComboEndTimer;

public:	
	UPlayerCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/* Toggles the character's active loadout between primary and secondary */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleActiveLoadout();

	/* Calls the ability to perform the loadout switch */
	void SwitchLoadout();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleWeaponSheathe();

	UFUNCTION(BlueprintPure, Category = "Combat")
	ECombatState GetCombatState() { return CombatState; };

	UFUNCTION(BlueprintPure, Category = "Combat")
	EMeleeComboType GetActiveMeleeComboType() { return ActiveMeleeComboType; };

	UFUNCTION(BlueprintPure, Category = "Combat")
	uint8 GetActiveMeleeComboCount() { return ActiveMeleeComboCount; };

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsMeleeComboReady() { return bIsMeleeComboReady; };

	/** Sets the characters combat state and replicates it to all clients. Calls appropriate multicast functions based on state. */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void ServerSetCombatState(ECombatState NewCombatSate);

	/** Server side function to set the active melee combo type, used for determining what melee ability to use next if the same attack type is repeated */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Combat")
	void ServerSetActiveMeleeComboType(EMeleeComboType ComboType);

	/** Sets the active melee combo type, used for determining what melee ability to use next if the same attack type is repeated */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Combat")
	void MulticastSetActiveMeleeComboType(EMeleeComboType ComboType);

	/** Server side function that clears the melee combo counter, used when switching to a different combo type */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Combat")
	void ServerClearMeleeComboCounter();

	/** Clears the melee combo counter, used when switching to a different combo type */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Combat")
	void MulticastClearMeleeComboCounter();

	/** Server side function that increments the melee combo counter, used for accessing the next melee ability when the same attack type is repeated */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Combat")
	void ServerIncrementMeleeComboCounter();

	/** Increments the melee combo counter, used for accessing the next melee ability when the same attack type is repeated */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Combat")
	void MulticastIncrementMeleeComboCounter();

	UFUNCTION()
	void PerformMainHandAttack();

	UFUNCTION()
	void PerformMainHandAltAttack();

	UFUNCTION()
	void PerformOffHandAttack();

	UFUNCTION()
	void PerformOffHandAltAttack();

	void TryPerformAttack(EMeleeComboType AttackType);

	void PerformAttack(TArray<TSubclassOf<UDungeonGameplayAbility>> Abilities, EMeleeComboType ComboType);
};
