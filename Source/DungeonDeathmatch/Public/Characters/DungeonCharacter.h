// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include <GameplayTagContainer.h>
#include "EquipmentComponent.h"
#include <GameplayTagAssetInterface.h>
#include "DungeonCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UWidgetComponent;
class USphereComponent;

class UDungeonAbilitySystemComponent;
class UDungeonAttributeSet;
class UDungeonGameplayAbility;
class UGameplayEffect;

class UInventoryComponent;
class UEquipmentComponent;

class AInteractableActor;
class AArmor;

class ACharacterRenderCapture2D;

/**
 * Enum for movement direction used for implementing certain gameplay abilities in blueprint
 */
UENUM(BlueprintType)
enum class ECardinalMovementDirection : uint8 {
	Forward			UMETA(DisplayName = "Forward"),
	ForwardLeft		UMETA(DisplayName = "Forward Left"),
	ForwardRight	UMETA(DisplayName = "Forward Right"),
	Left			UMETA(DisplayName = "Left"),
	Right			UMETA(DisplayName = "Right"),
	Backward		UMETA(DisplayName = "Backward"),
	BackwardLeft	UMETA(DisplayName = "BackwardLeft"),
	BackwardRight	UMETA(DisplayName = "BackwardRight")
};

/**
 * Enum representation of all available mesh segments for a character
 */
UENUM(BlueprintType)
enum class EMeshSegment : uint8
{
	Helm						UMETA(DisplayName = "Helm"),
	Hair						UMETA(DisplayName = "Hair"),
	Head						UMETA(DisplayName = "Head"),
	LeftShoulder				UMETA(DisplayName = "Left Shoulder"),
	RightShoulder				UMETA(DisplayName = "Right Shoulder"),
	Torso						UMETA(DisplayName = "Torso"),
	ChestArmor					UMETA(DisplayName = "ChestArmor"),
	LeftHand					UMETA(DisplayName = "LeftHand"),
	RightHand					UMETA(DisplayName = "RightHand"),
	Waist						UMETA(DisplayName = "Waist"),
	Legs						UMETA(DisplayName = "Legs"),
	LegArmor					UMETA(DisplayName = "LegArmor"),
	LeftFoot					UMETA(DisplayName = "LeftFoot"),
	RightFoot					UMETA(DisplayName = "RightFoot")
};

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

	/********************************************************* BEGIN CHARACTER MESH SEGMENT VARIABLES *********************************************************/

	/** The skeletal mesh component that stores the helm mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHelm;

	/** The skeletal mesh component that stores the hair mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHair;

	/** The skeletal mesh component that stores the head mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHead;

	/** The skeletal mesh component that stores the left shoulder mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderLeft;

	/** The skeletal mesh component that stores the right shoulder mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentShoulderRight;

	/** The skeletal mesh component that stores the torso mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentTorso;

	/** The skeletal mesh component that stores the chest armor mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentChestArmor;

	/** The skeletal mesh component that stores the left hand mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandLeft;

	/** The skeletal mesh component that stores the right hand mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentHandRight;

	/** The skeletal mesh component that stores the belt mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentBelt;

	/** The skeletal mesh component that stores the legs mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegs;

	/** The skeletal mesh component that stores the leg armor mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentLegArmor;

	/** The skeletal mesh component that stores the left mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootLeft;

	/** The skeletal mesh component that stores the right foot mesh segment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* MeshComponentFootRight;

	/** Mapping of mesh segment to mesh component, for updating the character mesh with new equipment */
	TMap<EMeshSegment, USkeletalMeshComponent*> MeshComponentMap;

	/********************************************************* END CHARACTER MESH SEGMENTS VARIABLES **********************************************************/

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

	/** 
	 * Abilities to grant to this character on creation for melee combos.
	 * These will be activated based on the current combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> UnarmedMeleeComboAbilities;

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

	/** The GameplayTag used to send custom melee hit events to hit actors */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	FGameplayTag UnarmedMeleeHitEventTag;

	/********************************************************* END GAMEPLAY ABILITY SYSTEM VARIABLES **********************************************************/

	/********************************************************* BEGIN INPUT VARIABLES **************************************************************************/

	/** Flag to determine if character is currently accepting movement input */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Input\|Movement")
	bool bIsMovementInputEnabled;

	/** Flag to determine if character is able to control the camera, typically disabled when in menus */
	UPROPERTY(BlueprintReadOnly, Category = "Input\|Camera")
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

	/********************************************************* BEGIN ANIMATION VARIABLES **********************************************************************/

	/** Flag to determine if character is in the middle of a jump, as opposed to just falling */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Animation\|Movement")
	bool bIsJumping;

	/** The minimum value to clamp the movement direction yaw to for blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Movement")
	float MovementDirectionYawClampMin;

	/** The maximum value to clamp the movement direction yaw to for blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Movement")
	float MovementDirectionYawClampMax;

	/** Flag to determine when to correct body orientation so it is facing the aim direction */
	UPROPERTY(Replicated)
	bool bIsReorientingBody;

	/** Flag to determine if character is currently manually free looking, affects turning and aim offsets */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Animation\|Aim")
	bool bIsManuallyFreeLooking;

	/** Flag to determine if character is currently auto free looking from an ability, affects turning and aim offsets */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Animation\|Aim")
	bool bIsAutoFreeLooking;

	/** The delta rotation yaw from the control rotation to the character rotation, used for aim offset blend spaces */
	UPROPERTY(Replicated)
	float AimYaw;

	/** The delta rotation pitch from the control rotation to the character rotation, used for aim offset blend spaces */
	UPROPERTY(Replicated)
	float AimPitch;

	/** Maximum yaw degrees the character can aim away from where their body is facing before the body will perform a correction turn to face the aim direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimYawTurnStart;

	/** Minimum yaw degrees the character can be aim away from where their body is facing during a correction turn before the turn will stop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimYawTurnStop;

	/** The minimum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimYawClampMin;

	/** The maximum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimYawClampMax;

	/** The minimum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimPitchClampMin;

	/** The maximum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation\|Aim")
	float AimPitchClampMax;

	/********************************************************* END ANIMATION VARIABLES ************************************************************************/

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

	/** The name of the socket corresponding to the left waist weapon sheathe */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameSheatheWaistLeft;

	/** The name of the socket corresponding to the right waist weapon sheathe */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameSheatheWaistRight;

	/** The name of the socket corresponding to the first back sheathe */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameSheatheBackOne;

	/** The name of the socket corresponding to the second back sheathe */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameSheatheBackTwo;

	/** The name of the socket corresponding to the first hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameConsumableOne;

	/** The name of the socket corresponding to the second hot keyed consumable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment\|Sockets")
	FString SocketNameConsumableTwo;

	/********************************************************* END INVENTORY & EQUIPMENT VARIABLES ************************************************************/

	/********************************************************* BEGIN COMBAT VARIABLES *************************************************************************/

	/** Volume for detecting unarmed attack hits with left fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderLeft;

	/** Volume for detecting unarmed attack hits with right fist */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderRight;

	/** Represents the index of the ability to use next in the MeleeCombatAbilities array for the current active weapon */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	uint8 CurrentMeleeComboState;

	/** Flag to determine if character can begin a new melee combo attack */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsMeleeComboReady;

	/********************************************************* END COMBAT VARIABLES ***************************************************************************/

	/** The 2DRenderCapture class for displaying the character mesh in the UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ACharacterRenderCapture2D> RenderCaptureClass;

	/** The 2DRenderCapture actor for displaying the character mesh in the UI */
	ACharacterRenderCapture2D* RenderCaptureActor;

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

	/********************************************************* BEGIN PUBLIC ANIMATION FUNCTIONS ***************************************************************/

	/**
	 * Gets the character's currently movement velocity, used for animation blend spaces
	 *
	 * @return The current movement velocity vector of this character
	 */
	UFUNCTION(BlueprintPure, Category = "Animation|\Movement")
	FVector GetMovementVelocity();

	/**
	 * Gets the character's current movement direction, used for animation blend spaces
	 *
	 * @return The angle of the character's movement direction, in degrees, from -180 to 180
	 */
	UFUNCTION(BlueprintPure, Category = "Animation|\Movement")
	float GetMovementDirection();

	/**
	 * Gets the enum representation of the current movement direction, a less specific value used for playing specific directional animation montages
	 *
	 * @return The enum representation of the closest cardinal movement direction
	 */
	UFUNCTION(BlueprintPure)
	ECardinalMovementDirection GetCardinalMovementDirection();

	/**
	 * Get if the character is currently jumping, as opposed to just falling.
	 *
	 * @return Whether the character is in the middle of a jump
	 */
	bool GetIsJumping();

	/**
	 * Set if the character is currently jumping, as opposed to just falling.
	 *
	 * @param IsJumping Is the character jumping?
	 */
	void SetIsJumping(bool IsJumping);

	/**
	 * Get the yaw of the character aim rotation
	 *
	 * @return The character's aim yaw
	 */
	UFUNCTION(BlueprintCallable, Category = "Animation|\Aim")
	float GetAimYaw();

	/**
	 * Get the pitch of the character aim rotation
	 *
	 * @return The character's aim pitch
	 */
	UFUNCTION(BlueprintCallable, Category = "Animation|\Aim")
	float GetAimPitch();

	/**
	 * Gets if free look is currently enabled for the character
	 *
	 * @return Is free look currently enabled for the character?
	 */
	UFUNCTION(BlueprintPure)
	bool GetIsFreeLooking();

	/**
	 * Sets if the character is manually free looking, as opposed to automatically through an ability
	 *
	 * @param IsManuallyFreeLooking Is the character manually free looking?
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsManuallyFreeLooking(bool IsManuallyFreeLooking);

	/**
	 * Sets if the character is automatically free looking through an ability, as opposed to manually through direct input
	 *
	 * @param IsAutoFreeLooking Is the character auto free looking?
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsAutoFreeLooking(bool IsAutoFreeLooking);

	/**
	 * Get if the character is currently reorienting its body. This happens when the character's aim yaw exceeds a given threshold.
	 *
	 * @return Whether the character is currently reorienting their body
	 */
	UFUNCTION(BlueprintCallable, Category = "Animation|\Aim")
	bool GetIsReorientingBody();

	/**
	 * BP Event for toggling the matching of actor rotation to controller rotation. Used for orienting the character body back towards the looking direction.
	 * This function only seems to be exposed in blueprint, so it needs to be overridden there.
	 *
	 * @param UseRotation Whether or not to use controller desired rotation
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation|\Aim")
	void UseControllerDesiredRotation(bool UseRotation);

	/********************************************************* END PUBLIC ANIMATION FUNCTIONS ********************************************************************/

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
	 * Attempts to add an item to the player's inventory. Only runs on the server.
	 *
	 * @param Item The item to attempt to add to the inventory
	 *
	 * @return Was the item added to the inventory?
	 */
	bool TryAddItemToInventory(AItem* Item);

	/**
	 * Attempts to pick up an item and add it to the player's inventory. Only runs on the server.
	 *
	 * @param Item The item to attempt to pick up
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryPickUpItem(AItem* Item);

	/**
	 * Attempts to remove an item from the player's inventory. Only runs on the server.
	 *
	 * @param Item The item to attempt to drop
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryDropItem(AItem* Item, FVector EjectionVector = FVector::ZeroVector);

	/**
	 * Attempts to equip an item and add it to the player's equipment. Only runs on the server.
	 *
	 * @param Equippable The item to attempt to equip
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryEquipItem(AEquippable* Equippable);

	/**
	 * Updates a character's mesh segments when armor equipment changes. Only called on the server.
	 *
	 * @param Armor The piece of armor to update meshes with
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UpdateMeshSegments(AArmor* Armor);

	/**
	 * Updates a character's mesh segments when armor equipment changes; called on all clients, should only be called by server.
	 *
	 * @param Armor The piece of armor to update meshes with
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UpdateMeshSegments(AArmor* Armor);

	/**
	 * Adds a weapon from a loadout slot to a specified sheathe location
	 *
	 * @param LoadoutSlot The struct containing the weapon and sheathe slot details
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UpdateLoadout(const FWeaponLoadout& Loadout);

	/********************************************************* END PUBLIC INVENTORY & EQUIPMENT FUNCTIONS *****************************************************/

	/********************************************************* BEGIN PUBLIC COMBAT FUNCTIONS ******************************************************************/

	/**
	 * Get the sphere component used for left fist collisions
	 *
	 * @return The left fist sphere component
	 */
	USphereComponent* GetLeftFistCollider();

	/**
	 * Get the sphere component used for right fist collisions
	 *
	 * @return The right fist sphere component
	 */
	USphereComponent* GetRightFistCollider();

	/**
	 * Sets flag to allow or disallow the character to perform their next melee combo attack.
	 * Only runs on the server.
	 *
	 * @param ComboReady Can perform next attack?
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_SetMeleeComboReady(bool ComboReady);

	/**
	 * Increases the melee combo state to determine the next attack ability to use.
	 * Only runs on the server.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_IncreaseMeleeComboState();

	/**
	 * Sets the melee combo state back to zero. This should happen after a full combo has been performed, or after
	 * the MeleeComboEndTimer expires.
	 * Only runs on the server.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_ResetMeleeComboState();

	/**
	 * Starts an internal timer that will disallow the character from performing the next combo attack once expired.
	 * Only runs on the server.
	 * @param TimeToComboEnd The amount of time, in seconds, to set the timer for
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_BeginMeleeComboEndTimer(float TimeToComboEnd);

	/********************************************************* END PUBLIC COMBAT FUNCTIONS ********************************************************************/

	/** 
	 * Gets the mapping of mesh segment to character's mesh component
	 * @return The mesh segment to component map
	 */
	TMap<EMeshSegment, USkeletalMeshComponent*> GetMeshComponentMap();

	/**
	 * Gets the 2DRenderCapture Actor for use by the UI
	 *
	 * @return The character's 2D RenderCapture Actor
	 */
	UFUNCTION(BlueprintCallable)
	ACharacterRenderCapture2D* GetRenderCaptureActor();

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

	/** Processes Sheathe key presses */
	UFUNCTION()
	void OnSheatheKeyPressed();

	/** Processes Attack key presses */
	UFUNCTION()
	void OnAttackKeyPressed();

	/** Processes Block key presses */
	UFUNCTION()
	void OnBlockKeyPressed();

	/** Processes Block key releases */
	UFUNCTION()
	void OnBlockKeyReleased();

	/** Processes Inventory key presses */
	UFUNCTION()
	void OnInventoryKeyPressed();

	/** Processes Escape key presses */
	UFUNCTION()
	void OnEscapeKeyPressed();

	/** Processes Use Inventory Item key presses. Only valid when the inventory menu is open. */
	UFUNCTION()
	void OnUseInventoryItemKeyPressed();

	/** Processes Drop Inventory Item key presses. Only valid when the inventory menu is open. */
	UFUNCTION()
	void OnDropInventoryItemKeyPressed();

	/********************************************************* END PROTECTED INPUT FUNCTIONS **************************************************************/

	/** Interacts with whatever the character is currently focusing. Only runs on the server. */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

	/********************************************************* BEGIN PROTECTED COMBAT FUNCTIONS **********************************************************/

	/** Sheathes the character's weapon. Only runs on the server. */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SheatheWeapon();

	/** Unsheathes the character's weapon. Only runs on the server. */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnsheatheWeapon();

	/**
	 * Gets whether or not the character is able to attack
	 *
	 * @return Can the character attack?
	 */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanAttack();

	/** Performs an attack. Only runs on the server. */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Combat")
	void Server_Attack();

	/** Cancels an attack. Only runs on the server. */
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

	/********************************************************* END PROTECTED COMBAT FUNCTIONS ************************************************************/

private:
	/** Calculates the character's aim yaw and pitch for use by aim offsets */
	void CalculateAimRotation();

	/**
	 * Attempts to equip an item and add it to the player's equipment. Will only be run on the server.
	 *
	 * @param Equippable The item to attempt to equip
	 *
	 * @ return Whether the item was successfully equipped
	 */
	bool TryEquipItem(AEquippable* Equippable);
};
