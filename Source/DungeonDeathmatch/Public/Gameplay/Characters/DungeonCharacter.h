// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <GameplayTagAssetInterface.h>
#include <GameplayTagContainer.h>

#include "DungeonCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;
class UBehaviorTree;

class UDungeonAbilitySystemComponent;
class UDungeonAttributeSet;
class UDungeonGameplayAbility;

UCLASS()
class DUNGEONDEATHMATCH_API ADungeonCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Friended to allow access to attribute modification handlers below
	friend class UDungeonAttributeSet;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	/** The component used to handle gameplay ability system interactions */
	UPROPERTY()
	UDungeonAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UDungeonAttributeSet* AttributeSet;

	UPROPERTY()
	bool bAbilitiesInitialized;

	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = "Abilities")
	int32 CharacterLevel;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GameplayAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartingGameplayEffects;

	/** The base movement speed when standing, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseStandingMovementSpeed;

	/** The base movement speed when crouched, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseCrouchedMovementSpeed;

public:
	ADungeonCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void InitializeAbilities();

	void InitializeMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UBehaviorTree* GetBehaviorTree() { return BehaviorTree; };

	/** Gets the character's AbilitySystemComponent, required by IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Adds an ability to the characters ability list */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	/** Populates a gameplay tag container with tags added to this object. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	/** Removes a gameplay tag from the character's tag container. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	bool RemoveGameplayTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxStamina() const;

	/** Gets the amount of stamina regenerated per second */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStaminaRegen() const;

	/** Gets current movement speed. Each point of movement speed corresponds to a 1% increase in character walk, run, and crouch speed. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMovementSpeed() const;

	/** Gets current movement speed multiplier. This is applied to a character's movement speed during things like sprinting and rolls, for temporary speed boosts. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMovementSpeedMultiplier() const;

	/** Gets the character level that is passed to the ability system. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual bool SetCharacterLevel(int32 NewLevel);

	/**
	 * BP Event for toggling the matching of actor rotation to controller rotation. Used for orienting the character body back towards the looking direction.
	 * This function only seems to be exposed in blueprint, so it needs to be called there.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Aim")
	void UseControllerDesiredRotation(bool UseRotation);

protected:
	/** Apply initial active and passive gameplay abilities to player. */
	void AddStartupGameplayAbilities();

	/** Attempts to remove any initial active and passive gameplay abilities from player. */
	void RemoveStartupGameplayAbilities();

	/**
	 * BP Event called when character takes damage.
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * BP Event called when health is changed.
	 *
	 * @param DeltaValue Change in health value
	 * @param EventTags The gameplay tags of the event that changed health
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * BP Event called when mana is changed.
	 *
	 * @param DeltaValue Change in mana value
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * BP Event called when stamina is changed.
	 *
	 * @param DeltaValue Change in stamina value
	 * @param EventTags The gameplay tags of the event that changed stamina
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * BP Event called when movement speed is changed
	 *
	 * @param DeltaValue Change in movement speed
	 * @param EventTags The gameplay tags of the event that changed movement speed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnMovementSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/** BP Event called when the player is killed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnDeath();

	/** Multicast function used to play effects all players need to see or hear on character death */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnDeath();

	virtual void NativeOnDeath();

	/**
	 * Native function called from DungeonAttributeSet when character takes damage.
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Native function called from DungeonAttributeSet when health is changed.
	 *
	 * @param DeltaValue Change in health value
	 * @param EventTags The gameplay tags of the event that changed health
	 */
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Native function called from DungeonAttributeSet when mana is changed.
	 *
	 * @param DeltaValue Change in mana value
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Native function called from DungeonAttributeSet stamina is changed.
	 *
	 * @param DeltaValue Change in stamina value
	 * @param EventTags The gameplay tags of the event that changed stamina
	 */
	virtual void HandleStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Native function called from DungeonAttributeSet movement speed is changed
	 *
	 * @param DeltaValue Change in movement speed
	 * @param EventTags The gameplay tags of the event that changed movement speed
	 */
	virtual void HandleMovementSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
};
