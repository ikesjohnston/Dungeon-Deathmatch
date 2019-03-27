// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DungeonCharacter.h"
#include "PlayerCharacter.generated.h"

class UWidgetComponent;

class UInventoryComponent;
class UEquipmentComponent;

class URenderCaptureComponent;
class ACharacterRenderCapture2D;
class UModularHumanoidMeshComponent;
class UCharacterAnimationComponent;
class UPlayerCombatComponent;

UCLASS()
class DUNGEONDEATHMATCH_API APlayerCharacter : public ADungeonCharacter
{
	GENERATED_BODY()

protected:
	/** Widget for displaying vitals to other players */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* VitalsPlateWidget;

	/** Component for controlling visuals of different mesh segments */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UModularHumanoidMeshComponent* ModularHumanoidMesh;

	/** Component that runs logic for variables used by the animation instance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UCharacterAnimationComponent* AnimationComponent;

	/** Component for controlling UI rendering of character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	URenderCaptureComponent* RenderCaptureComponent;

	/** Component for processing combat logic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UPlayerCombatComponent* PlayerCombatComponent;

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

	/** Flag to determine if character is currently accepting movement input */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Input|Movement")
	bool bIsMovementInputEnabled;

	/** Flag to determine if character is able to control the camera, typically disabled when in menus */
	UPROPERTY(BlueprintReadOnly, Category = "Input|Camera")
	bool bIsCameraInputEnabled;

	/** Component that stores a character currently has in their inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment")
	UInventoryComponent* InventoryComponent;

	/** Component that stores what a character currently has equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory & Equipment")
	UEquipmentComponent* EquipmentComponent;

private:
	/** Needed for removing and restoring deceleration during rolls */
	float DefaultWalkingDeceleration;

	/** Needed for removing and restoring ground friction during rolls */
	float DefaultGroundFriction;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	virtual void PostInitProperties() override;

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void InitializeAbilities() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent() { return InventoryComponent; };

	UFUNCTION(BlueprintPure)
	UEquipmentComponent* GetEquipmentComponent() { return EquipmentComponent; };

	UFUNCTION(BlueprintPure)
	UModularHumanoidMeshComponent* GetModularHumanoidMeshComponent() { return ModularHumanoidMesh; };

	UFUNCTION(BlueprintPure)
	UCharacterAnimationComponent* GetAnimationComponent() { return AnimationComponent; };

	UFUNCTION(BlueprintPure)
	URenderCaptureComponent* GetRenderCaptureComponent() { return RenderCaptureComponent; };

	UFUNCTION(BlueprintPure)
	ACharacterRenderCapture2D* GetRenderCaptureActor();

	UFUNCTION(BlueprintPure)
	UPlayerCombatComponent* GetCombatComponent() { return PlayerCombatComponent; };

	UFUNCTION(BlueprintCallable)
	bool GetIsMovementInputEnabled() { return bIsMovementInputEnabled; };

	UFUNCTION(BlueprintCallable)
	void SetIsMovementInputEnabled(bool IsMovementInputEnabled) { bIsMovementInputEnabled = IsMovementInputEnabled; };

	void SetIsCameraInputEnabled(bool IsCameraInputEnabled) { bIsCameraInputEnabled = IsCameraInputEnabled; };

protected:
	void NativeOnDeath() override;

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

	UFUNCTION()
	void OnJumpKeyPressed();
	UFUNCTION()
	void OnSprintKeyPressed();
	UFUNCTION()
	void OnSprintKeyReleased();
	UFUNCTION()
	void OnFreeLookKeyPressed();
	UFUNCTION()
	void OnFreeLookKeyReleased();
	UFUNCTION()
	void OnCrouchKeyPressed();
	UFUNCTION()
	void OnRollKeyPressed();
	UFUNCTION()
	void OnInteractKeyPressed();
	UFUNCTION()
	void OnLoadoutSwitchKeyPressed();
	UFUNCTION()
	void OnSheatheKeyPressed();
	UFUNCTION()
	void OnMainHandAttackKeyPressed();
	UFUNCTION()
	void OnMainHandAttackKeyReleased();
	UFUNCTION()
	void OnMainHandAltAttackKeyPressed();
	UFUNCTION()
	void OnMainHandAltAttackKeyReleased();
	UFUNCTION()
	void OnOffHandAttackKeyPressed();
	UFUNCTION()
	void OnOffHandAttackKeyReleased();
	UFUNCTION()
	void OnOffHandAltAttackKeyPressed();
	UFUNCTION()
	void OnOffHandAltAttackKeyReleased();
	UFUNCTION()
	void OnInventoryKeyPressed();
	UFUNCTION()
	void OnEscapeKeyPressed();

	/** Interacts with whatever the character is currently focusing. */
	UFUNCTION()
	void Interact();
};
