// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Equippable.h"
#include "EquipmentGlobals.h"
#include "WeaponTraceComponent.h"
#include "Weapon.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UBlendSpace;
class UBlendSpace1D;

class UDungeonGameplayAbility;

/**
 * The base class for all weapons in the game. Stores damaging effects and generates hit events for melee weapons when they are set in an attacking state.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AWeapon : public AEquippable
{
	GENERATED_BODY()

	friend class UWeaponTraceComponent;

protected:
	/* The visual representation of this  weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* WeaponMeshComponent;

	/** What hand(s) the weapon requires to use. Determines where the weapon is stored on the character and loadout validity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponHand WeaponHand;

	/** The weapon's type. Determines base animations and loadout validity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;

	/** The socket type this weapon uses when sheathed and drawn. Determined by the equipment slot it is in. Determines the sockets to attach the weapon to when equipped. */
	UPROPERTY(Replicated)
	EWeaponSocketType WeaponSocketType;
	
	/** Array of possible sounds to play when swinging the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> SwingSounds;

	/** Particle system to emit when swinging the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	UParticleSystem* SwingParticles;

	/** Particle system to emit when the weapon hits something. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	UParticleSystem* HitParticles;

	/** Array of possible sounds to play when the weapon hits metal. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> MetalHitSounds;

	/** Array of possible sounds to play when the weapon hits stone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> StoneHitSounds;

	/** Array of possible sounds to play when the weapon hits wood. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> WoodHitSounds;

	/** Array of possible sounds to play when the weapon hits leather. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> LeatherHitSounds;

	/** Array of possible sounds to play when the weapon hits cloth. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> ClothHitSounds;

	/** Array of possible sounds to play when the weapon hits flesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|\Effects")
	TArray<USoundCue*> FleshHitSounds;

	/**
	 * Abilities to grant to an equipping character for standard attacks when this weapon is in the main hand.
	 * These will be activated based on the current main hand standard attack combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> MainHandAbilities;

	/**
	 * Abilities to grant to an equipping character for alternate attacks when this weapon is in the main hand.
	 * These will be activated based on the current main hand alternate attack combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> MainHandAltAbilities;

	/**
	 * Abilities to grant to an equipping character for alternate attacks when this weapon is in the off hand.
	 * These will be activated based on the current off hand alternate attack combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> OffHandAltAbilities;

	/**
	 * Abilities to grant to an equipping character for standard attacks when this weapon is in the off hand.
	 * These will be activated based on the current off hand standard attack combo state.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> OffHandAbilities;

	/** The relative position adjustments for this weapon when attached to each sheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Attachment")
	TMap<EWeaponSocketType, FVector> SheathedSocketPositionAdjustments;

	/** The relative rotation adjustments for this weapon when attached to each sheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Attachment")
	TMap<EWeaponSocketType, FRotator> SheathedSocketRotationAdjustments;

	/** The relative position adjustments for this weapon when attached to each unsheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Attachment")
	TMap<EWeaponSocketType, FVector> UnsheathedSocketPositionAdjustments;

	/** The relative rotation adjustments for this weapon when attached to each unsheathed socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Attachment")
	TMap<EWeaponSocketType, FRotator> UnsheathedSocketRotationAdjustments;

	/** Any sheathing animation montages this weapon uses that override the default montages to use */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon\|Animation")
	TMap<EWeaponSocketType, UAnimMontage*> SheatheAnimationMontageOverrides;

	/** Any unsheathing animation montages this weapon uses that override the default montages to use */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon\|Animation")
	TMap<EWeaponSocketType, UAnimMontage*> UnsheatheAnimationMontageOverrides;

	/** Standing movement animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Animation")
	UBlendSpace* CombatStandingMovementBlendSpaceOverride;

	/** Crouching movement animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Animation")
	UBlendSpace* CombatCrouchingMovementBlendSpaceOverride;

	/** Jumping animation sequence to use when weapon is unsheathed, overrides any animation set by the equipping character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Animation")
	UAnimSequence* CombatJumpAnimationOverride;

	/** Falling animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Animation")
	UBlendSpace1D* CombatFallingBlendSpaceOverride;

	/** Landing animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon\|Animation")
	UBlendSpace* CombatLandingBlendSpaceOverride;

	TArray<AActor*> HitActorsThisSwing;
	TArray<AActor*> DamagedActorsThisSwing;
	TArray<AActor*> BlockingActorsThisSwing;

private:
	/** The currently spawned particle system component for an active swing. */
	UParticleSystemComponent* SwingParticleSystemComponent;

public:
	AWeapon(const FObjectInitializer& ObjectInitializer);

	virtual ~AWeapon() override;

protected:
	virtual void BeginPlay() override;

public:
	/** Gets the hand(s) required to use this weapon */
	UFUNCTION(BlueprintPure)
	EWeaponHand GetWeaponHand();

	/** Gets the type of the weapon */
	UFUNCTION(BlueprintPure)
	EWeaponType GetWeaponType();

	/** Gets The location where this weapon is sheathed. */
	UFUNCTION(BlueprintPure)
	EWeaponSocketType GetWeaponSocketType();

	/** Gets the sheathe animation montage to use for this weapon based on the current socket type */
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetSheatheAnimationMontage();

	/** Gets the sheathe animation montage to use for this weapon based on the current socket type */
	UFUNCTION(BlueprintPure)
	UAnimMontage* GetUnsheatheAnimationMontage();

	/** Gets the sheathed socket position adjustment vector based on the current socket type of the weapon */
	UFUNCTION(BlueprintPure)
	FVector GetSheathedSocketPositionAdjustment();

	/** Gets the sheathed socket rotation adjustment rotator based on the current socket type of the weapon */
	UFUNCTION(BlueprintPure)
	FRotator GetSheathedSocketRotationAdjustment();

	/** Gets the unsheathed socket position adjustment vector based on the current socket type of the weapon */
	UFUNCTION(BlueprintPure)
	FVector GetUnsheathedSocketPositionAdjustment();

	/** Gets the unsheathed socket rotation adjustment rotator based on the current socket type of the weapon */
	UFUNCTION(BlueprintPure)
	FRotator GetUnsheathedSocketRotationAdjustment();

	/** Get the standing movement animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UFUNCTION(BlueprintPure, Category = "Animation\|Movement")
	UBlendSpace* GetCombatStandingMovementBlendSpaceOverride();

	/** Get the crouching movement animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UFUNCTION(BlueprintPure, Category = "Animation\|Movement")
	UBlendSpace* GetCombatCrouchingMovementBlendSpaceOverride();

	/** Get the jumping animation sequence to use when weapon is unsheathed, overrides any animation set by the equipping character */
	UFUNCTION(BlueprintPure, Category = "Animation\|Movement")
	UAnimSequence* GetCombatJumpAnimationOverride();

	/** Get the falling animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UFUNCTION(BlueprintPure, Category = "Animation\|Movement")
	UBlendSpace1D* GetCombatFallingBlendSpaceOverride();

	/** Get the landing animation blend space to use when weapon is unsheathed, overrides any blend spaces set by the equipping character */
	UFUNCTION(BlueprintPure, Category = "Animation\|Movement")
	UBlendSpace* GetCombatLandingBlendSpaceOverride();

	/** Gets the abilities to grant to an equipping character for standard main hand attacks. */
	UFUNCTION(BlueprintCallable, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GetMainHandAbilities();

	/** Gets the abilities to grant to an equipping character for alternate main hand attacks. */
	UFUNCTION(BlueprintCallable, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GetMainHandAltAbilities();

	/** Gets the abilities to grant to an equipping character for standard off hand attacks. */
	UFUNCTION(BlueprintCallable, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GetOffHandAbilities();

	/** Gets the abilities to grant to an equipping character for alternate off hand attacks. */
	UFUNCTION(BlueprintCallable, Category = "Weapon\|Abilities")
	TArray<TSubclassOf<UDungeonGameplayAbility>> GetOffHandAltAbilities();

	/** Start a weapon swing, playing any effects and tracing for hits.*/
	void StartSwing();

	/** Stop a weapon swing.*/
	void StopSwing();

protected:
	virtual void ServerOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot) override;
	virtual void MulticastOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot) override;

	virtual void ServerOnUnequip_Implementation() override;
	virtual void MulticastOnUnequip_Implementation() override;

	void OnHitDetected(FWeaponHitResult WeaponHitResult);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnHitDetected(FWeaponHitResult WeaponHitResult);

	UFUNCTION(Server, Reliable, WithValidation)
	void MulticastOnHitDetected(FWeaponHitResult WeaponHitResult);
};
