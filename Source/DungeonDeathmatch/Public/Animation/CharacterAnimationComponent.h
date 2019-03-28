// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "EquipmentGlobals.h"
#include "CharacterAnimationComponent.generated.h"

class UBlendSpace;
class UBlendSpace1D;
class UAnimSequence;
class ADungeonCharacter;

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UCharacterAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Flag to determine if character is in the middle of a jump, as opposed to just falling */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsJumping;

	/** The minimum value to clamp the movement direction yaw to for blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementDirectionYawClampMin;

	/** The maximum value to clamp the movement direction yaw to for blend spaces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementDirectionYawClampMax;

	/** Standing movement animation blend space to use when weapons are sheathed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UBlendSpace* DefaultStandingMovementBlendSpace;

	/** Crouching movement animation blend space to use when weapons are sheathed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UBlendSpace* DefaultCrouchingMovementBlendSpace;

	/** Jumping animation sequence to use when weapons are sheathed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UAnimSequence* DefaultJumpingAnimation;

	/** Falling animation blend space to use when weapons are sheathed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UBlendSpace1D* DefaultFallingBlendSpace;

	/** Landing animation blend space to use when weapons are sheathed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UBlendSpace* DefaultLandingBlendSpace;

	/** Mapping of blend spaces to use for standing movement animations when weapons are unsheathed based on loadout type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TMap<ELoadoutType, UBlendSpace*> CombatStandingMovementBlendSpaceMap;

	/** Mapping of blend spaces to use for crouching movement animations when weapons are unsheathed based on loadout type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TMap<ELoadoutType, UBlendSpace*> CombatCrouchingMovementBlendSpaceMap;

	/** Mapping of animation sequences to use for jumping when weapons are unsheathed based on loadout type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TMap<ELoadoutType, UAnimSequence*> CombatJumpingAnimationMap;

	/** Mapping of blend spaces to use for falling animations when weapons are unsheathed based on loadout type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TMap<ELoadoutType, UBlendSpace1D*> CombatFallingBlendSpaceMap;

	/** Mapping of blend spaces to use for landing animations when weapons are unsheathed based on loadout type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TMap<ELoadoutType, UBlendSpace*> CombatLandingBlendSpaceMap;

	/** Flag to determine when to correct body orientation so it is facing the aim direction */
	UPROPERTY(Replicated)
	bool bIsReorientingBody;

	/** Flag to determine if character is currently manually free looking, affects turning and aim offsets */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Aim")
	bool bIsManuallyFreeLooking;

	/** Flag to determine if character is currently auto free looking from an ability, affects turning and aim offsets */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Aim")
	bool bIsAutoFreeLooking;

	/** The delta rotation yaw from the control rotation to the character rotation, used for aim offset blend spaces */
	UPROPERTY(Replicated)
	float AimYaw;

	/** The delta rotation pitch from the control rotation to the character rotation, used for aim offset blend spaces */
	UPROPERTY(Replicated)
	float AimPitch;

	/** Maximum yaw degrees the character can aim away from where their body is facing before the body will perform a correction turn to face the aim direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimYawTurnStart;

	/** Minimum yaw degrees the character can be aim away from where their body is facing during a correction turn before the turn will stop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimYawTurnStop;

	/** The minimum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimYawClampMin;

	/** The maximum value to clamp the aim rotation yaw to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimYawClampMax;

	/** The minimum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimPitchClampMin;

	/** The maximum value to clamp the aim rotation pitch to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimPitchClampMax;

private:
	/** The player character that this component is attached to */
	ADungeonCharacter* OwningCharacter;

public:	
	// Sets default values for this component's properties
	UCharacterAnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Gets the character's currently movement velocity, used for animation blend spaces
	 *
	 * @return The current movement velocity vector of this character
	 */
	UFUNCTION(BlueprintPure, Category = "Movement")
	FVector GetMovementVelocity();

	/**
	 * Gets the character's current movement direction, used for animation blend spaces
	 *
	 * @return The angle of the character's movement direction, in degrees, from -180 to 180
	 */
	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetMovementDirection();

	/**
	 * Gets the enum representation of the current movement direction, a less specific value used for playing specific directional animation montages
	 *
	 * @return The enum representation of the closest cardinal movement direction
	 */
	UFUNCTION(BlueprintPure)
	ECardinalMovementDirection GetCardinalMovementDirection();

	/** Gets the standing movement animation blend space to use when weapons are sheathed*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetDefaultStandingMovementBlendSpace();

	/** Gets the crouching movement animation blend space to use when weapons are sheathed*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetDefaultCrouchingMovementBlendSpace();

	/** Gets the jumping animation sequence to use when weapons are sheathed*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UAnimSequence* GetDefaultJumpingAnimation();

	/** Gets the falling animation blend space to use when weapons are sheathed*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace1D* GetDefaultFallingBlendSpace();

	/** Gets the landing animation blend space to use when weapons are sheathed*/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetDefaultLandingBlendSpace();

	/** Gets the blend space to use for standing movement animations when weapons are unsheathed based on loadout type */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetCombatStandingMovementBlendSpace();

	/** Gets the blend space to use for crouching movement animations when weapons are unsheathed based on loadout type */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetCombatCrouchingMovementBlendSpace();

	/** Gets the animation sequence to use for jumping when weapons are unsheathed based on loadout type */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UAnimSequence* GetCombatJumpingAnimation();

	/** Gets the blend space to use for falling animations when weapons are unsheathed based on loadout type */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace1D* GetCombatFallingBlendSpace();

	/** Gets the blend space to use for landing animations when weapons are unsheathed based on loadout type */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBlendSpace* GetCombatLandingBlendSpace();

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
	UFUNCTION(BlueprintCallable, Category = "Aim")
	float GetAimYaw();

	/**
	 * Get the pitch of the character aim rotation
	 *
	 * @return The character's aim pitch
	 */
	UFUNCTION(BlueprintCallable, Category = "Aim")
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
	UFUNCTION(BlueprintCallable, Category = "Aim")
	bool GetIsReorientingBody();
		
private:
	/** Calculates the character's aim yaw and pitch for use by aim offsets */
	void CalculateAimRotation();
};
