// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <GameplayTagContainer.h>
#include "EquipmentGlobals.h"
#include <GameplayTagAssetInterface.h>
#include "InventoryGlobals.h"
#include "EquipmentGlobals.h"
#include "DungeonCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UWidgetComponent;

class UDungeonAbilitySystemComponent;
class UDungeonAttributeSet;
class UDungeonGameplayAbility;
class UGameplayEffect;

class UInventoryComponent;
class UEquipmentComponent;

class AInteractableActor;
class AArmor;

class ACharacterRenderCapture2D;
class UModularCharacterMeshComponent;
class UPlayerCombatComponent;
class URenderCaptureComponent;
class UCharacterAnimationComponent;

// OnArmorEquipped event, currently just being used for UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorEquippedSignature, AArmor*, Armor);

/**
 * Character class that encapsulates player input and attribute change processing.
 */
UCLASS()
class DUNGEONDEATHMATCH_API ADungeonCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Friended to allow access to attribute modification handlers below
	friend class UDungeonAttributeSet;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnArmorEquippedSignature OnArmorEquipped;

protected:
	/** The player camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	/** The spring arm that the player camera is attached to; checks for camera collisions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	/** Widget for displaying vitals to other players */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* VitalsPlateWidget;

	/** Component for controlling visuals of different mesh segments */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UModularCharacterMeshComponent* ModularCharacterMesh;

	/** Component that runs logic for variables used by the animation instance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UCharacterAnimationComponent* AnimationComponent;

	/** Component for controlling UI rendering of character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	URenderCaptureComponent* RenderCaptureComponent;

	/** Component for processing combat logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UPlayerCombatComponent* PlayerCombatComponent;

	/********************************************************* BEGIN GAMEPLAY ABILITY SYSTEM VARIABLES ********************************************************/

	/** The component used to handle gameplay ability system interactions */
	UPROPERTY()
	UDungeonAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UDungeonAttributeSet* AttributeSet;

	/** Whether a character's abilities have been initialized */
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

	/** The GameplayAbility to use when pressing the free look key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StartFreeLookAbility;

	/** The GameplayAbility to use when releasing the free look key */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StopFreeLookAbility;

	/********************************************************* END GAMEPLAY ABILITY SYSTEM VARIABLES **********************************************************/

	/********************************************************* BEGIN INPUT VARIABLES **************************************************************************/

	/** Flag to determine if character is currently accepting movement input */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Input|Movement")
	bool bIsMovementInputEnabled;

	/** Flag to determine if character is able to control the camera, typically disabled when in menus */
	UPROPERTY(BlueprintReadOnly, Category = "Input|Camera")
	bool bIsCameraInputEnabled;

	/********************************************************* END INPUT VARIABLES ****************************************************************************/

	/********************************************************* BEGIN MOVEMENT VARIABLES ***********************************************************************/

	/** The base movement speed when standing, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseStandingMovementSpeed;

	/** The base movement speed when crouched, used to calculate new movement speeds when speed attributes are changed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseCrouchedMovementSpeed;

	/********************************************************* END MOVEMENT VARIABLES *************************************************************************/

	/********************************************************* BEGIN INVENTORY & EQUIPMENT VARIABLES **********************************************************/

	/** Component that stores a character currently has in their inventory */
	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
	UInventoryComponent* InventoryComponent;

	/** Component that stores what a character currently has equipped */
	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
	UEquipmentComponent* EquipmentComponent;

	/** The relative location to drop items */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment")
	USceneComponent* ItemDropLocation;

	/** The force to apply to items when dropping them */
	UPROPERTY(EditAnywhere, Category = "Inventory & Equipment")
	float DropEjectionForce;

	/** Mapping of sock types to socket names, used for attaching weapons to the character mesh during equipment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment")
	TMap<EWeaponSocketType, FName> WeaponSocketMap;

	/** The name of the socket corresponding to the first hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment|Sockets")
	FString SocketNameConsumableOne;

	/** The name of the socket corresponding to the second hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment|Sockets")
	FString SocketNameConsumableTwo;

	/********************************************************* END INVENTORY & EQUIPMENT VARIABLES ************************************************************/

private:
	/** Needed for removing and restoring deceleration during rolls */
	float DefaultWalkingDeceleration;

	/** Needed for removing and restoring ground friction during rolls */
	float DefaultGroundFriction;

public:
	// Sets default values for this character's properties
	ADungeonCharacter();

protected:
	virtual void PostInitProperties() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	/********************************************************* BEGIN PUBLIC ABILITY SYSTEM FUNCTIONS **********************************************************/
 
	/**
	 * Gets the character's AbilitySystemComponent, required by IAbilitySystemInterface
	 *
	 * @return The character's AbilitySystemComponent
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/**
	 * Adds an ability to the characters ability list
	 *
	 * @param Ability The gameplay ability to give to the character
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	/**
	 * Gets current health, will be 0 if dead
	 *
	 * @return The current health of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetHealth() const;

	/**
	 * Gets maximum health, health will never be greater than this
	 *
	 * @return The max health of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxHealth() const;

	/**
	 * Gets current mana
	 *
	 * @return The current mana of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMana() const;

	/**
	 *  Gets maximum mana, mana will never be greater than this
	 *
	 * @return The max mana of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxMana() const;

	/**
	 * Gets current stamina
	 *
	 * @return The current stamina of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStamina() const;

	/**
	 * Gets maximum stamina, stamina will never be greater than this
	 *
	 * @return The max stamina of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMaxStamina() const;

	/**
	 * Gets the amount of stamina regenerated per second
	 *
	 * @return The stamina regen rate of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetStaminaRegen() const;

	/**
	 * Gets current movement speed. Each point of movement speed corresponds to a 1% increase in character walk, run, and crouch speed.
	 *
	 * @return The movement speed of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMovementSpeed() const;

	/**
	 * Gets current movement speed multiplier. This is applied to a character's movement speed during things like sprinting and rolls, for temporary speed boosts.
	 *
	 * @return The current movement speed multiplier of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual float GetMovementSpeedMultiplier() const;

	/**
	 * Gets the character level that is passed to the ability system
	 *
	 * @return The level of this character
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual int32 GetCharacterLevel() const;

	/**
	 * Modifies the character level, this may change abilities. Returns true on success
	 *
	 * @param NewLevel The new character level
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual bool SetCharacterLevel(int32 NewLevel);

	/********************************************************* END PUBLIC ABILITY SYSTEM FUNCTIONS *************************************************************/

	/********************************************************* END PUBLIC GAMEPLAY TAG ASSET FUNCTIONS *************************************************************/

	/**
	 * Populates a gameplay tag container with tags added to this object.
	 *
	 * @param TagContainer The reference to the tag container to populate
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	/**
	 * Removes a gameplay tag from the character's tag container.
	 *
	 * @param Tag The gameplay tag to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	bool RemoveGameplayTag(FGameplayTag Tag);

	/********************************************************* END PUBLIC GAMEPLAY TAG ASSET FUNCTIONS *************************************************************/

	/********************************************************* BEGIN PUBLIC INPUT FUNCTIONS *******************************************************************/

	/**
	 * Get if the character is currently accepting movement input
	 *
	 * @return Whether movement input is currently enabled
	 */
	UFUNCTION(BlueprintCallable)
	bool GetIsMovementInputEnabled();

	/**
	 * Set if the character is currently accepting movement input
	 *
	 * @param IsJumping Is the character accepting movement input?
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsMovementInputEnabled(bool IsMovementInputEnabled);

	/**
	 * Set if the character can control the camera. Typically used to disable camera input while in menus.
	 *
	 * @param CanLook Can the character control the camera?
	 */
	void SetIsCameraInputEnabled(bool IsCameraInputEnabled);

	/********************************************************* END PUBLIC INPUT FUNCTIONS *********************************************************************/

	/**
	 * BP Event for toggling the matching of actor rotation to controller rotation. Used for orienting the character body back towards the looking direction.
	 * This function only seems to be exposed in blueprint, so it needs to be called there.
	 *
	 * @param UseRotation Whether or not to use controller desired rotation
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Aim")
	void UseControllerDesiredRotation(bool UseRotation);

	/********************************************************* BEGIN PUBLIC INVENTORY & EQUIPMENT FUNCTIONS ******************************************************/
	/**
	 * Gets the character's InventoryComponent
	 *
	 * @return The character's InventoryComponent
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory & Equipment")
	UInventoryComponent* GetInventoryComponent();

	/**
	 * Gets the character's EquipmentComponent
	 *
	 * @return The character's EquipmentComponent
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory & Equipment")
	UEquipmentComponent* GetEquipmentComponent();

	/**
	 * Server side function that attempts to add an item to the character's inventory and makes a multicast RPC with the result. Used for items that are currently "despawned" and may only be visible from UI elements.
	 *
	 * @param Item The item to attempt to add to the inventory
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_RequestAddItemToInventory(AItem* Item);

	/**
	 * Server side function that attempts to add an item to the character's inventory at the specified location and makes a multicast RPC with the result. Used for items that are currently "despawned" and may only be visible from UI elements.
	 *
	 * @param Item The item to attempt to add to the inventory
	 * @param OriginSlot The upper left most grid slot where the item should be placed
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_RequestAddItemToInventoryAtLocation(AItem* Item, FInventoryGridPair OriginSlot);

	/**
	 * Server side function that attempts to pick up an item and add it to the character's inventory and makes a multicast RPC with the result. Used when interacting with items in the world.
	 *
	 * @param Item The item to attempt to pick up
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestPickUpItem(AItem* Item);

	/**
	 * Server side function that attempts to remove an item from the character's inventory and makes a multicast RPC with the result. Used for items that should remain "despawned" and only be visible from UI elements.
	 *
	 * @param Item The item to attempt to remove from the inventory
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestRemoveItemFromInventory(AItem* Item);

	/**
	 * Server side function that attempts to remove an item from the character's inventory and "spawn" it in front of the character and makes a multicast RPC with the result.
	 *
	 * @param Item The item to attempt to drop
	 * @param CheckInventory Whether to check if the item is in the inventory before dropping it. This should only not be done when dropping via Drag & Drop operation from the inventory
	 * in which case the item has already been removed from the inventory.
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestDropItem(AItem* Item, bool CheckInventory = true);

	/**
	 * Server side function that attempts to equip an item to any valid equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param TryMoveReplacementToInventory Should an attempt be made to move any item already in the specified slot back to the character's inventory?
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestEquipItem(AEquippable* Equippable, bool TryMoveReplacementToInventory = false);

	/**
	 * Server side function that attempts to equip an item to a specific equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to equip
	 * @param EquipmentSlot The equipment slot to attempt to put the item into
	 * @param TryMoveReplacementToInventory Should an attempt be made to move any item already in the specified slot back to the character's inventory?
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestEquipItemToSlot(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveReplacementToInventory = false);

	/**
	 * Server side function that attempts to unequip an item from a specific equipment slot and makes a multicast RPC with the result. Can be used when interacting with items in the world or in the UI.
	 *
	 * @param Equippable The item to attempt to unequip
	 * @param EquipmentSlot The equipment slot to attempt to remove the item from
	 * @param TryMoveToInventory Should an attempt be made to move the unequipped item back to the character's inventory?
	 */
	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation, Category = "Inventory & Equipment")
	void Server_RequestUnequipItem(AEquippable* Equippable, EEquipmentSlot EquipmentSlot, bool TryMoveToInventory = false);

	/**
	 * Server call to attach an actor to the character mesh at the specified socket.
	 *
	 * @param Actor The actor to attach
	 * @param SocketName The name of the socket to attach the actor to
	 * @param RelativePosition The relative position adjustment to transform the actor by after attachment
	 * @param RelativeRotation The relative rotation adjustment to transform the actor by after attachment
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/**
	 * Server call to detach an actor from the character mesh.
	 *
	 * @param Actor The actor to detach
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DetachActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	UModularCharacterMeshComponent* GetModularCharacterMeshComponent();

	UFUNCTION(BlueprintCallable)
	UCharacterAnimationComponent* GetAnimationComponent();

	UFUNCTION(BlueprintCallable)
	URenderCaptureComponent* GetRenderCaptureComponent();

	UFUNCTION(BlueprintCallable)
	ACharacterRenderCapture2D* GetRenderCaptureActor();

	UFUNCTION(BlueprintCallable)
	UPlayerCombatComponent* GetCombatComponent();

	/** Gets socket name on the character for a given weapon socket type*/
	FName GetNameForWeaponSocket(EWeaponSocketType WeaponSocketType);

	/********************************************************* END PUBLIC INVENTORY & EQUIPMENT FUNCTIONS *****************************************************/

protected:
	/********************************************************* BEGIN PROTECTED ABILITY SYSTEM FUNCTIONS *******************************************************/

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

	/** Multicast function used to play effects all players need to see or hear on player death */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

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

	/********************************************************* END PROTECTED ABILITY SYSTEM FUNCTIONS *******************************************************/

	/********************************************************* BEGIN PROTECTED INPUT FUNCTIONS **************************************************************/

	/**
	 * Adds forward movement input to the character movement component
	 * 
	 * @param Value The magnitude of the forward movement input to add, from -1 to 1
	 */
	void MoveForward(float Value);

	/**
	 * Adds right movement input to the character movement component
	 *
	 * @param Value The magnitude of the right movement input to add, from -1 to 1
	 */
	void MoveRight(float Value);

	/**
	 * Adds yaw input to the character's camera
	 *
	 * @param Value The magnitude of the yaw input to add, from -1 to 1
	 */
	void LookRight(float YawInput);
	
	/**
	 * Adds pitch input to the character's camera
	 *
	 * @param Value The magnitude of the pitch input to add, from -1 to 1
	 */
	void LookUp(float PitchInput);

	/** Makes the character jump on the next update. Also sets bIsJumping to true. */
	virtual void Jump() override;

	/** Processes Jump key presses */
	UFUNCTION()
	virtual void OnJumpKeyPressed();

	/** Processes Sprint key presses */
	UFUNCTION()
	void OnSprintKeyPressed();

	/** Processes Sprint key releases */
	UFUNCTION()
	void OnSprintKeyReleased();

	/** Processes Free Look key presses */
	UFUNCTION()
	void OnFreeLookKeyPressed();

	/** Processes Free Look key releases */
	UFUNCTION()
	void OnFreeLookKeyReleased();

	/** Processes Crouch key presses */
	UFUNCTION()
	void OnCrouchKeyPressed();

	/** Processes Roll key presses */
	UFUNCTION()
	void OnRollKeyPressed();

	/** Processes Interact key presses */
	UFUNCTION()
	void OnInteractKeyPressed();

	/** Processes LoadoutSwitch key presses */
	UFUNCTION()
	void OnLoadoutSwitchKeyPressed();

	/** Processes Sheathe key presses */
	UFUNCTION()
	void OnSheatheKeyPressed();

	/** Processes Main Hand Attack key presses */
	UFUNCTION()
	void OnMainHandAttackKeyPressed();

	/** Processes Main Hand Attack key releases */
	UFUNCTION()
	void OnMainHandAttackKeyReleased();

	/** Processes Main Hand Alt Attack key presses */
	UFUNCTION()
	void OnMainHandAltAttackKeyPressed();

	/** Processes Main Hand Alt Attack key releases */
	UFUNCTION()
	void OnMainHandAltAttackKeyReleased();

	/** Processes Off Hand Attack key presses */
	UFUNCTION()
	void OnOffHandAttackKeyPressed();

	/** Processes Off Hand Attack key releases */
	UFUNCTION()
	void OnOffHandAttackKeyReleased();

	/** Processes Off Hand Alt Attack key presses */
	UFUNCTION()
	void OnOffHandAltAttackKeyPressed();

	/** Processes Off Hand Alt Attack key releases */
	UFUNCTION()
	void OnOffHandAltAttackKeyReleased();

	/** Processes Inventory key presses */
	UFUNCTION()
	void OnInventoryKeyPressed();

	/** Processes Escape key presses */
	UFUNCTION()
	void OnEscapeKeyPressed();

	/********************************************************* END PROTECTED INPUT FUNCTIONS **************************************************************/

	/** Interacts with whatever the character is currently focusing. */
	UFUNCTION()
	void Interact();

	/********************************************************* BEGIN PROTECTED INVENTORY & EQUIPMENT FUNCTIONS ************************************************************/

	/**
	 * Multicast function that delivers the response from a server request to add an item to the character's inventory. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param Item The item that was requested to be added to the inventory
	 * @param WasAdded Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AddItemToInventoryResponse(AItem* Item, bool WasAdded);

	/**
	 * Multicast function that delivers the response from a server request to pick up an item. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param Item The item that was requested to be picked up
	 * @param WasPickedUp Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PickUpItemResponse(AItem* Item, bool WasPickedUp);

	/**
	 * Multicast function that delivers the response from a server request to remove an item from the character's inventory. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param Item The item that was requested to be removed from the inventory
	 * @param WasRemoved Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_RemoveItemFromInventoryResponse(AItem* Item, bool WasRemoved);

	/**
	 * Multicast function that delivers the response from a server request to drop an item. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param Item The item that was requested to be dropped
	 * @param WasDropped Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DropItemResponse(AItem* Item, bool WasDropped);

	/**
	 * Multicast function that delivers the response from a server request to equip an item. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param Equippable The item that was requested to be equipped
	 * @param WasEquipped Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_EquipItemResponse(AEquippable* Equippable, bool WasEquipped);

	/**
	 * Multicast function that delivers the response from a server request to unequip an item. Used to trigger any required UI events. Only processed for locally controlled characters.
	 *
	 * @param  Equippable The item that was requested to be unequipped
	 * @param WasUnequipped Was the request successful?
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UnequipItemResponse(AEquippable* Equippable, bool WasUnequipped);

	/**
	 * Attaches an actor to the character mesh at the specified socket. Should only be called by the server.
	 *
	 * @param Actor The actor to attach
	 * @param SocketName The name of the socket to attach the actor to
	 * @param RelativePosition The relative position adjustment to transform the actor by after attachment
	 * @param RelativeRotation The relative rotation adjustment to transform the actor by after attachment
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation);

	/**
	 * Detaches an actor from the character mesh. Should only be called by the server.
	 *
	 * @param Actor The actor to detach
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DetachActor(AActor* Actor);

	/********************************************************* END PROTECTED INVENTORY & EQUIPMENT FUNCTIONS ************************************************************/
};
