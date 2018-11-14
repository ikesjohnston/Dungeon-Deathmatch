// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnimationProfile.h"
#include "AbilitySystemInterface.h"
#include "DungeonGameplayAbility.h"
#include "DungeonCharacter.generated.h"

class USceneComponent;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UStatusComponent;
class AWeapon;
class UInventoryComponent;
class UEquipmentComponent;
class UDungeonAbilitySystemComponent;

/**
 * Enum that maps gameplay abilities to the action mappings defined in project settings. The enum index corresponds to the
 * ability input ID, and the enum name corresponds to the action mapping name.
 */  
UENUM(BlueprintType) enum class AbilityInput : uint8 {
	Sprint		UMETA(DisplayName = "Sprint Ability"),
	Roll		UMETA(DisplayName = "Roll Ability"),
};

/*
 * Combo Attack State Enum
 * Until a more advanced melee combo system is put in place, each
 * weapon will simply have a three attack combo of preset animations.
 * This enum represents the current state of a combo attack.
 */
UENUM(BlueprintType)
enum class EComboState : uint8
{
	NotStarted,
	AttackOne,
	AttackTwo,
	AttackThree
};

UCLASS()
class DUNGEONDEATHMATCH_API ADungeonCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	// Needed for removing and restoring deceleration during rolls
	float DefaultWalkingDeceleration;
	// Needed for removing and restoring ground friction during rolls
	float DefaultGroundFriction;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStatusComponent* StatusComponent;

	/*
	 * The component used to handle gameplay ability system interactions 
	 */
	UPROPERTY()
	UDungeonAbilitySystemComponent* AbilitySystemComponent;

	/*
	 * Volume for detecting unarmed attack hits with left fist
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderLeft;

	/*
	 * Volume for detecting unarmed attack hits with right fist
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	USphereComponent* FistColliderRight;

	/*
	 * The amount of stamina it costs per second for the player to sprint
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintStaminaCost;

	/*
	 * The amount of stamina it costs per second for the player to sprint
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RollStaminaCost;

	/*
	 * The amount of stamina it costs per second for the player to sprint
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpStaminaCost;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsRolling;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimationProfile* AnimationProfile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StartSprintAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UDungeonGameplayAbility> StopSprintAbility;

	/*UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	EComboState ComboState;

	FTimerHandle ComboTimer;*/

public:
	// Sets default values for this character's properties
	ADungeonCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_Tick(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Jump();

	void MoveForward(float Value);

	void MoveRight(float Value);

	virtual void Jump() override;

	UFUNCTION()
	void OnSprintPressed();

	UFUNCTION()
	void OnSprintReleased();

	UFUNCTION(BlueprintCallable)
	void BeginSprint();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_BeginSprint();

	UFUNCTION(BlueprintCallable)
	void EndSprint();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_EndSprint();

	UFUNCTION(BlueprintCallable)
	void ToggleCrouch();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ToggleCrouch();

	UFUNCTION(BlueprintCallable)
	void BeginRoll();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_BeginRoll();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_EndRoll();

	UFUNCTION()
	void OnUsePressed();

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Use();

	void OnSheathePressed();

	void OnAttackPressed();

	/*
	 * Checks if a character is in a valid state to perform an attack
	 */
	UFUNCTION(BlueprintPure)
	bool CanAttack();

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack();

	UFUNCTION(BlueprintCallable)
	void CancelAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CancelAttack();

	/*UFUNCTION(BlueprintCallable)
	void SetComboReady();*/

	UFUNCTION()
	void OnHealthChanged(UStatusComponent* OwningStatusComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	UFUNCTION(NetMulticast, Reliable)

	/*
	 * Plays an animation montage on the server and every client
	 */
	void Multicast_PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	/*
	 * Stops an animation montage on the server and every client
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopAllAnimMontages();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

public:

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	void EndRoll();

	UFUNCTION(BlueprintCallable)
	void SetCanRoll(bool CanRoll);

	UFUNCTION(BlueprintCallable)
	void SheatheWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SheatheWeapon();

	UFUNCTION(BlueprintCallable)
	void UnsheatheWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnsheatheWeapon();

	UAnimationProfile* GetAnimationProfile();
};
