// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <GameplayTagContainer.h>
#include "DungeonCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UAnimationProfile;
class UDungeonAbilitySystemComponent;
class UDungeonAttributeSet;
class UDungeonGameplayAbility;
class UInventoryComponent;
class UEquipmentComponent;
class USphereComponent;
class UWidgetComponent;
class UGameplayEffect;

/**
 * Enum that maps gameplay abilities to the action mappings defined in project settings. The enum index corresponds to the
 * ability input ID, and the enum name corresponds to the action mapping name.
 */  
UENUM(BlueprintType) enum class AbilityInput : uint8 {
	Sprint		UMETA(DisplayName = "Sprint Ability"),
	Roll		UMETA(DisplayName = "Roll Ability"),
};

/** Character class that encapsulates player input and attribute change processing. */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Friended to allow access to attribute modification handlers below
	friend class UDungeonAttributeSet;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	/** The component used to handle gameplay ability system interactions */
	UPROPERTY()
	UDungeonAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UEquipmentComponent* EquipmentComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UDungeonAttributeSet* AttributeSet;

	/** If true we have initialized our abilities */
	UPROPERTY()
	bool bAbilitiesInitialized;

	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = "Abilities")
	int32 CharacterLevel;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GameplayAbilities;

	/** Abilities to grant to this character on creation for melee combos.
	 *  These will be activated based on the current combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> UnarmedMeleeComboAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** Volume for detecting unarmed attack hits with left fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderLeft;

	/** Volume for detecting unarmed attack hits with right fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderRight;

	/** 3D Widget for displaying enemy health */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthPlateWidget;

	/** Internal crouch flag for determining movement state transitions */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching;

	/** Internal sprinting flag for determining movement state transitions */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	/** Internal rolling flag for determining movement state transitions */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsRolling;

	/** Internal attacking flag for determining combo state transitions */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsAttackInProgress;

	/** Internal rolling flag for determining movement state transitions */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bCanRoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MaxWalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MaxSprintingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MaxCrouchedWalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MaxRollingSpeed;

	/** Defines all animation montages a player uses in a given situation. This should be updated based on equipment.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimationProfile* AnimationProfile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StartSprintAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StopSprintAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> JumpAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> CrouchAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> RollAbility;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	FGameplayTag UnarmedMeleeHitEventTag;

	/** Represents the index of the ability to use next in the MeleeCombatAbilities array for the current active weapon */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	uint8 CurrentMeleeComboState;

	/** Flag to determine if character can begin a new melee combo attack */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsMeleeComboReady;

private:
	/** Needed for removing and restoring deceleration during rolls */
	float DefaultWalkingDeceleration;

	/** Needed for removing and restoring ground friction during rolls */
	float DefaultGroundFriction;

	/** Timer used to end a combo a defined amount of time after an attack ends */
	FTimerHandle MeleeComboEndTimer;

public:
	// Sets default values for this character's properties
	ADungeonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/**
	 * Adds an ability to the characters ability list
	 *
	 * @param Ability The gameplay ability to give to the character
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	/**
	* Attempts to activate all abilities that match the specified tags
	* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UDungeonGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxHealth() const;

	/** Returns current mana */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxMana() const;

	/** Returns current stamina */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStamina() const;

	/** Returns maximum stamina, stamina will never be greater than this */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxStamina() const;

	/** Returns current stamina regen rate*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStaminaRegen() const;

	/** Returns current movement speed */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMoveSpeed() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual bool SetCharacterLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void EndRoll();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetCanRoll(bool CanRoll);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SheatheWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SheatheWeapon();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UnsheatheWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnsheatheWeapon();

	/** 
	 * Sets flag to allow or disallow the character to perform their next melee combo attack.
	 * Only runs on the server.
	 * @param ComboReady Can perform next attack?
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_SetMeleeComboReady(bool ComboReady);

	/**
	 * Inreases the melee combo state to determine the next attack ability to use.
	 * Only runs on the server.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_IncreaseMeleeComboState();

	/**
	 * Starts an internal timer that will disallow the character from performing the next combo attack once expired.
	 * Only runs on the server.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_BeginMeleeComboEndTimer(float TimeToComboEnd);

	/**
	 * Sets the melee combo state back to zero. This should happen after a full combo has been performed, or after
	 * the MeleeComboEndTimer expires.
	 * Only runs on the server.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_ResetMeleeComboState();

	UAnimationProfile* GetAnimationProfile();

	USphereComponent* GetLeftFistCollider();

	USphereComponent* GetRightFistCollider();

protected:
	/** Apply the startup gameplay abilities and effects */
	void AddStartupGameplayAbilities();

	/** Attempts to remove any startup gameplay abilities */
	void RemoveStartupGameplayAbilities();

	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called when mana is changed, either from healing or from being used as a cost
	 *
	 * @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called when stamina is changed, either from healing or from being used as a cost
	 *
	 * @param DeltaValue Change in stamina value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed stamina
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called when movement speed is changed
	 *
	 * @param DeltaValue Change in move speed
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes")
	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	// Called from DungeonAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Jump();

	void MoveForward(float Value);

	void MoveRight(float Value);

	virtual void Jump() override;

	UFUNCTION()
	void OnSprintPressed();

	UFUNCTION()
	void OnSprintReleased();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void BeginSprint();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_BeginSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void EndSprint();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_EndSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ToggleCrouch();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void BeginRoll();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_BeginRoll();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_EndRoll();

	UFUNCTION()
	void OnUsePressed();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Use();

	void OnSheathePressed();

	void OnAttackPressed();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanAttack();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_Attack();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_CancelAttack();

	/** Stops all animation montages on the server and every client */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopAllAnimMontages();

	/** Processes overlap events for the character's left fist during a melee attack. Will only be processed on the server.*/
	UFUNCTION()
	void OnFistColliderLeftBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Processes overlap events for the character's right fist during a melee attack. Will only be processed on the server.*/
	UFUNCTION()
	void OnFistColliderRightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Sends an Damage.Melee.Unarmed Gameplay Event to an Actor hit by this character's unarmed attack. Will only be processed on the server.
	 *
	 * @param HitActor The Actor hit by this character's unarmed attack.
	 */
	UFUNCTION()
	void SendUnarmedMeleeHitEvent(AActor* HitActor);
};
