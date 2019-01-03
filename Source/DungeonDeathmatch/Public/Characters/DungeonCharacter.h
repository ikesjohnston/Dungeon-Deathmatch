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
class AInteractableActor;

/**
 * Enum that maps gameplay abilities to the action mappings defined in project settings. The enum index corresponds to the
 * ability input ID, and the enum name corresponds to the action mapping name.
 */  
UENUM(BlueprintType) enum class AbilityInput : uint8 {
	Sprint		UMETA(DisplayName = "Sprint Ability"),
	Roll		UMETA(DisplayName = "Roll Ability"),
};

/**
 * Enum for movement direction used for implementing certain gaeplay abilities in blueprint.
 */
UENUM(BlueprintType) enum class ECardinalMovementDirection : uint8 {
	Forward			UMETA(DisplayName = "Forward"),
	ForwardLeft		UMETA(DisplayName = "Forward Left"),
	ForwardRight	UMETA(DisplayName = "Forward Right"),
	Left			UMETA(DisplayName = "Left"),
	Right			UMETA(DisplayName = "Right"),
	Backward		UMETA(DisplayName = "Backward"),
	BackwardLeft	UMETA(DisplayName = "BackwardLeft"),
	BackwardRight	UMETA(DisplayName = "BackwardRight"),
};

UENUM(BlueprintType)
enum class EMeshSegment : uint8
{
	Head						UMETA(DisplayName = "Head"),
	LeftShoulder				UMETA(DisplayName = "Left Shoulder"),
	RightShoulder				UMETA(DisplayName = "Right Shoulder"),
	Chest						UMETA(DisplayName = "Chest"),
	Waist						UMETA(DisplayName = "Waist"),
	Legs						UMETA(DisplayName = "Legs"),
	LeftFoot					UMETA(DisplayName = "LeftFoot"),
	RightFoot					UMETA(DisplayName = "RightFoot"),
	LeftHand					UMETA(DisplayName = "LeftHand"),
	RightHand					UMETA(DisplayName = "RightHand")
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

	// -------------------- Begin Character Mesh Segments --------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHelm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentTorso;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentChestArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentBelt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootRight;
	// -------------------- End Character Mesh Segments ----------------------

	/** The component used to handle gameplay ability system interactions */
	UPROPERTY()
	UDungeonAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
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
	TArray<TSubclassOf<UGameplayEffect>> StartingGameplayEffects;

	/** Volume for detecting unarmed attack hits with left fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderLeft;

	/** Volume for detecting unarmed attack hits with right fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderRight;

	/** 3D Widget for displaying enemy health */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthPlateWidget;

	/** Defines all animation montages a player uses in a given situation. This should be updated based on equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimationProfile* AnimationProfile;

	/** The GameplayAbility to use when pressing the sprint key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StartSprintAbility;

	/** The GameplayAbility to use when sprinting must stop, either from releasing the sprint key or external factors */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StopSprintAbility;

	/** The GameplayAbility to use when pressing the jump key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> JumpAbility;

	/** The GameplayAbility to use when pressing the crouch key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> CrouchAbility;

	/** The GameplayAbility to use when pressing the roll key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> RollAbility;

	/** The GameplayTag used to send custom melee hit events to hit actors */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	FGameplayTag UnarmedMeleeHitEventTag;

	/** Represents the index of the ability to use next in the MeleeCombatAbilities array for the current active weapon */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	uint8 CurrentMeleeComboState;

	/** Flag to determine if character can begin a new melee combo attack */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsMeleeComboReady;

	/** The base movement speed when standing, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseStandingMovementSpeed;

	/** The base movement speed when crouched, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseCrouchedMovementSpeed;

	/** Flag to determine if character is currently accepting movement input */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsMovementInputEnabled;

	/** Flag to determine if character is in the middle of a jump, as opposed to just falling */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsJumping;

	bool bCanLook;

	// -------------------------------------------- Begin Aim Offset Calculation Variables --------------------------------------------
	/* Used to determine when to correct body orientation so it is facing the aim direction */
	UPROPERTY(Replicated)
	bool bIsReorientingBody;

	/* The delta rotation yaw from the control rotation to the character rotation, used for aim offset blendspaces */
	UPROPERTY(Replicated)
	float AimYaw;

	/* The delta rotation pitch from the control rotation to the character rotation, used for aim offset blendspaces */
	UPROPERTY(Replicated)
	float AimPitch;

	/** Maximum yaw degrees the character can aim away from where their body is facing before the body will perform a correction turn to face the aim direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimYawTurnStart;

	/** Minimum yaw degrees the character can be aim away from where their body is facing during a correction turn before the turn will stop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimYawTurnStop;

	/* The minimum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimYawClampMin;

	/* The maximum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimYawClampMax;

	/* The minimum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimPitchClampMin;

	/* The maximum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float AimPitchClampMax;
	// -------------------------------------------- End Aim Offset Calculation Variables --------------------------------------------

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

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintPure)
	UEquipmentComponent* GetEquipmentComponent();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	FVector GetMovementVelocity();

	UFUNCTION(BlueprintPure)
	float GetMovementDirection();

	UFUNCTION(BlueprintPure)
	ECardinalMovementDirection GetCardinalMovementDirection();

	/**
	 * Get if the character is currently accepting movement input
	 */
	UFUNCTION(BlueprintCallable)
	bool GetIsMovementInputEnabled();

	/**
	 * Set if the character is currently accepting movement input
	 * @param IsJumping Is the character accepting movement input?
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsMovementInputEnabled(bool IsMovementInputEnabled);

	/**
	 * Get if the character is currently jumping, as opposed to just falling.
	 */
	bool GetIsJumping();

	/**
	 * Set if the character is currently jumping, as opposed to just falling.
	 * @param IsJumping Is the character jumping?
	 */
	void SetIsJumping(bool IsJumping);

	/**
	 * Set if the character can control the camera. Typically used to disable camera input while in menus.
	 * @param CanLook Can the character look around?
	 */
	void SetCanLook(bool CanLook);

	/**
	 * Adds an ability to the characters ability list
	 *
	 * @param Ability The gameplay ability to give to the character
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability);

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
	virtual float GetMovementSpeed() const;

	/** Returns current movement speed multiplier*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMovementSpeedMultiplier() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual bool SetCharacterLevel(int32 NewLevel);

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

	/**
	 * Attempts to pick up an item and add it to the player's inventory
	 * @param Item The item to attempt to pick up
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryPickUpItem(AItem* Item);

	/**
	 * Attempts to remove an item from the player's inventory
	 * @param Item The item to attempt to drop
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryDropItem(AItem* Item);

	UFUNCTION(BlueprintCallable)
	bool GetIsReorientingBody();

	UFUNCTION(BlueprintCallable)
	float GetAimYaw();

	UFUNCTION(BlueprintCallable)
	float GetAimPitch();

	/* Event for toggling the matching of actor rotation to controller rotation. Used for orienting the character body back towards the looking direction. */
	UFUNCTION(BlueprintImplementableEvent)
	void UseControllerDesiredRotation(bool UseRotation);

	void UpdateMeshSegments(TMap<EMeshSegment, USkeletalMesh*> MeshMap);

protected:
	/** Apply initial acitve and passive gameplay abilities to player. */
	void AddStartupGameplayAbilities();

	/** Attempts to remove any initial acitve and passive gameplay abilities from player. */
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

	/** Multicast function used to play effects all players need to see or hear on player death */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	// Called from DungeonAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ADungeonCharacter* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleStaminaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleMovementSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void LookRight(float YawInput);
	
	void LookUp(float PitchInput);

	virtual void OnJumpKeyPressed();

	/**
	 * Makes the character jump on the next update. Also sets bIsJumping to true.
	 */
	virtual void Jump() override;

	UFUNCTION()
	void OnSprintKeyPressed();

	UFUNCTION()
	void OnSprintKeyReleased();

	UFUNCTION()
	void OnCrouchKeyPressed();

	UFUNCTION()
	void OnRollKeyPressed();

	UFUNCTION()
	void OnInteractKeyPressed();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

	UFUNCTION()
	void OnSheatheKeyPressed();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SheatheWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnsheatheWeapon();

	UFUNCTION()
	void OnAttackKeyPressed();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanAttack();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_Attack();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_CancelAttack();

	/** Processes overlap events for the character's left fist during a melee attack. Will only be processed on the server.*/
	UFUNCTION()
	void OnFistColliderLeftBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Processes overlap events for the character's right fist during a melee attack. Will only be processed on the server.*/
	UFUNCTION()
	void OnFistColliderRightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/**
	 * Sends a melee hit Gameplay Event to an Actor hit by this character's unarmed attack. Will only be processed on the server.
	 *
	 * @param HitActor The Actor hit by this character's unarmed attack.
	 */
	UFUNCTION()
	void SendUnarmedMeleeHitEvent(AActor* HitActor);

	UFUNCTION()
	void OnBlockKeyPressed();

	UFUNCTION()
	void OnBlockKeyReleased();

	UFUNCTION()
	void OnInventoryKeyPressed();

	UFUNCTION()
	void OnEscapeKeyPressed();

	UFUNCTION()
	void OnUseInventoryItemKeyPressed();

	UFUNCTION()
	void OnDropInventoryItemKeyPressed();

private:
	void CalculateAimRotation();
};
