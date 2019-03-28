// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimationComponent.h"
#include "DungeonCharacter.h"
#include "PlayerCharacter.h"
#include "EquipmentComponent.h"
#include "EquipmentGlobals.h"

#include <GameFramework/MovementComponent.h>

#define CARDINAL_MOVEMENT_FORWARD_MAX 22.5f
#define CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX 67.5f
#define CARDINAL_MOVEMENT_RIGHT_MAX 112.5f
#define CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX 157.5f
#define CARDINAL_MOVEMENT_BACKWARD 180.0f

// Sets default values for this component's properties
UCharacterAnimationComponent::UCharacterAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;

	OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerCombatComponent::UPlayerCombatComponent - OwningCharacter is null."))
			return;
	}

	MovementDirectionYawClampMin = -179.0f;
	MovementDirectionYawClampMax = 180.0f;

	AimYawTurnStart = 60;
	AimYawTurnStop = 5;
	AimYawClampMin = -120;
	AimYawClampMax = 120;
	AimPitchClampMin = -90;
	AimPitchClampMax = 90;

	bIsAutoFreeLooking = false;
	bIsManuallyFreeLooking = false;
}

void UCharacterAnimationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterAnimationComponent, AimYaw);
	DOREPLIFETIME(UCharacterAnimationComponent, AimPitch);
	DOREPLIFETIME(UCharacterAnimationComponent, bIsReorientingBody);
	DOREPLIFETIME(UCharacterAnimationComponent, bIsManuallyFreeLooking);
	DOREPLIFETIME(UCharacterAnimationComponent, bIsAutoFreeLooking);
	DOREPLIFETIME(UCharacterAnimationComponent, bIsJumping);
}

void UCharacterAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Have server calculate aim offset
	// TODO: This will need to be updated to use client prediction for smoothness at some point, with some defined acceptable error range
	if (OwningCharacter->HasAuthority())
	{
		CalculateAimRotation();
	}

	// Set jumping back to false after hitting the ground, used by animation instances
	if (bIsJumping && !OwningCharacter->GetCharacterMovement()->IsFalling())
	{
		SetIsJumping(false);
	}

	APlayerCharacter* OwningPlayer = Cast<APlayerCharacter>(OwningCharacter);
	if (OwningPlayer)
	{
		OwningPlayer->UseControllerDesiredRotation(!(bIsManuallyFreeLooking || bIsAutoFreeLooking));
	}
}

FVector UCharacterAnimationComponent::GetMovementVelocity()
{
	FVector Velocity;

	UCharacterMovementComponent* MovementComp = OwningCharacter->GetCharacterMovement();
	if (MovementComp)
	{
		Velocity = MovementComp->Velocity;
	}
	else
	{
		Velocity = FVector(0, 0, 0);
	}

	return Velocity;
}

float UCharacterAnimationComponent::GetMovementDirection()
{
	float MovementDirection = 0.0f;

	FVector Velocity = GetMovementVelocity();
	if (Velocity.Size() > 0)
	{
		FRotator ActorRotation = OwningCharacter->GetActorRotation();
		FVector LocalVelocity = ActorRotation.UnrotateVector(Velocity);
		MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	}
	else
	{
		// If standing still, the aim direction should act as the move direction, this is used for rolling
		// If free aiming, however, move direction should be forward
		if (GetIsFreeLooking())
		{
			MovementDirection = 0;
		}
		else
		{
			FRotator DeltaRotation = OwningCharacter->GetControlRotation() - OwningCharacter->GetActorRotation();
			MovementDirection = FMath::ClampAngle(DeltaRotation.Yaw, MovementDirectionYawClampMin, MovementDirectionYawClampMax);
		}
	}

	return MovementDirection;
}

ECardinalMovementDirection UCharacterAnimationComponent::GetCardinalMovementDirection()
{
	float MovementDirection = GetMovementDirection();

	if (MovementDirection >= -CARDINAL_MOVEMENT_FORWARD_MAX && MovementDirection <= CARDINAL_MOVEMENT_FORWARD_MAX)
	{
		return ECardinalMovementDirection::Forward;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_FORWARD_MAX && MovementDirection <= CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::ForwardRight;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_RIGHT_MAX)
	{
		return ECardinalMovementDirection::Right;
	}
	else if (MovementDirection > CARDINAL_MOVEMENT_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::BackwardRight;
	}
	else if ((MovementDirection > CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX && MovementDirection <= CARDINAL_MOVEMENT_BACKWARD) ||
		(MovementDirection >= -CARDINAL_MOVEMENT_BACKWARD && MovementDirection < -CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX))
	{
		return ECardinalMovementDirection::Backward;
	}
	else if (MovementDirection >= -CARDINAL_MOVEMENT_BACKWARD_RIGHT_MAX && MovementDirection < -CARDINAL_MOVEMENT_RIGHT_MAX)
	{
		return ECardinalMovementDirection::BackwardLeft;
	}
	else if (MovementDirection >= -CARDINAL_MOVEMENT_RIGHT_MAX && MovementDirection < -CARDINAL_MOVEMENT_FORWARD_RIGHT_MAX)
	{
		return ECardinalMovementDirection::Left;
	}
	else
	{
		return ECardinalMovementDirection::ForwardLeft;
	}
}

bool UCharacterAnimationComponent::GetIsJumping()
{
	return bIsJumping;
}

void UCharacterAnimationComponent::SetIsJumping(bool IsJumping)
{
	bIsJumping = IsJumping;
}

bool UCharacterAnimationComponent::GetIsFreeLooking()
{
	bool IsFreeLooking = (bIsManuallyFreeLooking || bIsAutoFreeLooking);
	return IsFreeLooking;
}

void UCharacterAnimationComponent::SetIsManuallyFreeLooking(bool IsManuallyFreeLooking)
{
	bIsManuallyFreeLooking = IsManuallyFreeLooking;
	OwningCharacter->UseControllerDesiredRotation(!(bIsManuallyFreeLooking || bIsAutoFreeLooking));
}

void UCharacterAnimationComponent::SetIsAutoFreeLooking(bool IsAutoFreeLooking)
{
	bIsAutoFreeLooking = IsAutoFreeLooking;
	OwningCharacter->UseControllerDesiredRotation(!(bIsManuallyFreeLooking || bIsAutoFreeLooking));
}

bool UCharacterAnimationComponent::GetIsReorientingBody()
{
	return bIsReorientingBody;
}

UBlendSpace* UCharacterAnimationComponent::GetDefaultStandingMovementBlendSpace()
{
	return DefaultStandingMovementBlendSpace;
}

UBlendSpace* UCharacterAnimationComponent::GetDefaultCrouchingMovementBlendSpace()
{
	return DefaultCrouchingMovementBlendSpace;
}

UAnimSequence* UCharacterAnimationComponent::GetDefaultJumpingAnimation()
{
	return DefaultJumpingAnimation;
}

UBlendSpace1D* UCharacterAnimationComponent::GetDefaultFallingBlendSpace()
{
	return DefaultFallingBlendSpace;
}

UBlendSpace* UCharacterAnimationComponent::GetDefaultLandingBlendSpace()
{
	return DefaultLandingBlendSpace;
}

UBlendSpace* UCharacterAnimationComponent::GetCombatStandingMovementBlendSpace()
{
	UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(OwningCharacter->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (CombatComponent && CombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(OwningCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

			if (ActiveLoadout.MainHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatStandingMovementBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}
			else if (ActiveLoadout.OffHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatStandingMovementBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}

			ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
			UBlendSpace** BlendSpacePtr = CombatStandingMovementBlendSpaceMap.Find(LoadoutType);
			if (BlendSpacePtr)
			{
				return *BlendSpacePtr;
			}
		}
	}

	return DefaultStandingMovementBlendSpace;
}

UBlendSpace* UCharacterAnimationComponent::GetCombatCrouchingMovementBlendSpace()
{
	UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(OwningCharacter->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (CombatComponent && CombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(OwningCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

			if (ActiveLoadout.MainHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatCrouchingMovementBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}
			else if (ActiveLoadout.OffHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatCrouchingMovementBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}

			ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
			UBlendSpace** BlendSpacePtr = CombatCrouchingMovementBlendSpaceMap.Find(LoadoutType);
			if (BlendSpacePtr)
			{
				return *BlendSpacePtr;
			}
		}
	}

	return DefaultCrouchingMovementBlendSpace;
}

UAnimSequence* UCharacterAnimationComponent::GetCombatJumpingAnimation()
{
	UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(OwningCharacter->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (CombatComponent && CombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(OwningCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

			if (ActiveLoadout.MainHandWeapon)
			{
				UAnimSequence* AnimSequence = ActiveLoadout.MainHandWeapon->GetCombatJumpAnimationOverride();
				if (AnimSequence)
				{
					return AnimSequence;
				}
			}
			else if (ActiveLoadout.OffHandWeapon)
			{
				UAnimSequence* AnimSequence = ActiveLoadout.OffHandWeapon->GetCombatJumpAnimationOverride();
				if (AnimSequence)
				{
					return AnimSequence;
				}
			}

			ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
			UAnimSequence** AnimSequencePtr = CombatJumpingAnimationMap.Find(LoadoutType);
			if (AnimSequencePtr)
			{
				return *AnimSequencePtr;
			}
		}
	}

	return DefaultJumpingAnimation;
}

UBlendSpace1D* UCharacterAnimationComponent::GetCombatFallingBlendSpace()
{
	UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(OwningCharacter->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (CombatComponent && CombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(OwningCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

			if (ActiveLoadout.MainHandWeapon)
			{
				UBlendSpace1D* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatFallingBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}
			else if (ActiveLoadout.OffHandWeapon)
			{
				UBlendSpace1D* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatFallingBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}

			ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
			UBlendSpace1D** BlendSpacePtr = CombatFallingBlendSpaceMap.Find(LoadoutType);
			if (BlendSpacePtr)
			{
				return *BlendSpacePtr;
			}
		}
	}

	return DefaultFallingBlendSpace;
}

UBlendSpace* UCharacterAnimationComponent::GetCombatLandingBlendSpace()
{
	UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(OwningCharacter->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
	if (CombatComponent && CombatComponent->GetCombatState() != ECombatState::Sheathed)
	{
		UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(OwningCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));
		if (EquipmentComponent)
		{
			FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();

			if (ActiveLoadout.MainHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.MainHandWeapon->GetCombatLandingBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}
			else if (ActiveLoadout.OffHandWeapon)
			{
				UBlendSpace* BlendSpace = ActiveLoadout.OffHandWeapon->GetCombatLandingBlendSpaceOverride();
				if (BlendSpace)
				{
					return BlendSpace;
				}
			}

			ELoadoutType LoadoutType = UDungeonEquipmentLibrary::GetLoadoutType(ActiveLoadout);
			UBlendSpace** BlendSpacePtr = CombatLandingBlendSpaceMap.Find(LoadoutType);
			if (BlendSpacePtr)
			{
				return *BlendSpacePtr;
			}
		}
	}

	return DefaultLandingBlendSpace;
}

float UCharacterAnimationComponent::GetAimYaw()
{
	return AimYaw;
}

float UCharacterAnimationComponent::GetAimPitch()
{
	return AimPitch;
}

void UCharacterAnimationComponent::CalculateAimRotation()
{
	FRotator ControlRotation = OwningCharacter->GetControlRotation();
	FRotator ActorRotation = OwningCharacter->GetActorRotation();

	FRotator DeltaRotation = ControlRotation - ActorRotation;
	AimYaw = FMath::ClampAngle(DeltaRotation.Yaw, AimYawClampMin, AimYawClampMax);
	AimPitch = FMath::ClampAngle(DeltaRotation.Pitch, AimPitchClampMin, AimPitchClampMax);

	UCharacterMovementComponent* MovementComp = OwningCharacter->GetCharacterMovement();
	if (MovementComp)
	{
		FVector Velocity = MovementComp->Velocity;

		if (Velocity.Size() > 0 && !GetIsFreeLooking())
		{
			// No aim offsets when moving unless aiming a bow/spell or free looking
			bIsReorientingBody = false;
			OwningCharacter->UseControllerDesiredRotation(true);
		}
		else
		{
			if (!bIsReorientingBody)
			{
				if (FMath::Abs(AimYaw) >= AimYawTurnStart && !GetIsFreeLooking())
				{
					bIsReorientingBody = true;
					OwningCharacter->UseControllerDesiredRotation(true);
				}
				else
				{
					// Allow aim offsets when not moving
					OwningCharacter->UseControllerDesiredRotation(false);
				}
			}
			else if (bIsReorientingBody && FMath::Abs(AimYaw) <= AimYawTurnStop)
			{
				bIsReorientingBody = false;
			}
		}
	}
}
