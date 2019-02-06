// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include "EquipmentGlobals.h"
#include "Equippable.h"
#include "Item.h"
#include "InteractTooltipWidget.h"
#include "ItemTooltipWidget.h"
#include "DungeonGameInstance.h"
#include "DungeonCharacter.h"

// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	DamagingVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DamagingVolume"));
	DamagingVolume->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	DamagingVolume->SetCollisionResponseToChannel(TRACE_INTERACTABLE, ECR_Ignore);
	DamagingVolume->SetupAttachment(RootMeshComponent);
}

AWeapon::~AWeapon()
{

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponSocketType);
}

EWeaponHand AWeapon::GetWeaponHand()
{
	return WeaponHand;
}

EWeaponType AWeapon::GetWeaponType()
{
	return WeaponType;
}

EWeaponSocketType AWeapon::GetWeaponSocketType()
{
	return WeaponSocketType;
}

UAnimMontage* AWeapon::GetSheatheAnimationMontage()
{
	UAnimMontage** MontagePtr = SheatheAnimationMontageOverrides.Find(WeaponSocketType);
	if (MontagePtr)
	{
		return *MontagePtr;
	}
	else
	{
		UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			MontagePtr = GameInstance->GetSheatheAnimationMontages().Find(WeaponSocketType);
			if (MontagePtr)
			{
				return *MontagePtr;
			}
		}
	}

	return nullptr;
}

UAnimMontage* AWeapon::GetUnsheatheAnimationMontage()
{
	UAnimMontage** MontagePtr = UnsheatheAnimationMontageOverrides.Find(WeaponSocketType);
	if (MontagePtr)
	{
		return *MontagePtr;
	}
	else
	{
		UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			MontagePtr = GameInstance->GetUnsheatheAnimationMontages().Find(WeaponSocketType);
			if (MontagePtr)
			{
				return *MontagePtr;
			}
		}
	}

	return nullptr;
}

FVector AWeapon::GetSheathedSocketPositionAdjustment()
{
	FVector* VectorPtr = SheathedSocketPositionAdjustments.Find(WeaponSocketType);
	if (VectorPtr)
	{
		return *VectorPtr;
	}
	return FVector::ZeroVector;
}

FRotator AWeapon::GetSheathedSocketRotationAdjustment()
{
	FRotator* RotatorPtr = SheathedSocketRotationAdjustments.Find(WeaponSocketType);
	if (RotatorPtr)
	{
		return *RotatorPtr;
	}
	return FRotator::ZeroRotator;
}

FVector AWeapon::GetUnsheathedSocketPositionAdjustment()
{
	FVector* VectorPtr = UnsheathedSocketPositionAdjustments.Find(WeaponSocketType);
	if (VectorPtr)
	{
		return *VectorPtr;
	}
	return FVector::ZeroVector;
}

FRotator AWeapon::GetUnsheathedSocketRotationAdjustment()
{
	FRotator* RotatorPtr = UnsheathedSocketRotationAdjustments.Find(WeaponSocketType);
	if (RotatorPtr)
	{
		return *RotatorPtr;
	}
	return FRotator::ZeroRotator;
}

UBlendSpace* AWeapon::GetCombatStandingMovementBlendSpaceOverride()
{
	return CombatStandingMovementBlendSpaceOverride;
}

UBlendSpace* AWeapon::GetCombatCrouchingMovementBlendSpaceOverride()
{
	return CombatCrouchingMovementBlendSpaceOverride;
}

UAnimSequence* AWeapon::GetCombatJumpAnimationOverride()
{
	return CombatJumpAnimationOverride;
}

UBlendSpace1D* AWeapon::GetCombatFallingBlendSpaceOverride()
{
	return CombatFallingBlendSpaceOverride;
}

UBlendSpace* AWeapon::GetCombatLandingBlendSpaceOverride()
{
	return CombatLandingBlendSpaceOverride;
}

void AWeapon::ServerOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::ServerOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);

	switch (EquipmentSlot)
	{
	case EEquipmentSlot::WeaponLoadoutOneMainHand:
		if (WeaponHand == EWeaponHand::OneHand)
		{
			WeaponSocketType = EWeaponSocketType::MainHandWeaponLoadoutOne;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::TwoHandWeaponLoadoutOne;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutOneOffHand:
		if (WeaponType == EWeaponType::Shield)
		{
			WeaponSocketType = EWeaponSocketType::OffHandShieldLoadoutOne;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::OffHandWeaponLoadoutOne;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutTwoMainHand:
		if (WeaponHand == EWeaponHand::OneHand)
		{
			WeaponSocketType = EWeaponSocketType::MainHandWeaponLoadoutTwo;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::TwoHandWeaponLoadoutTwo;
		}
		break;
	case EEquipmentSlot::WeaponLoadoutTwoOffHand:
		if (WeaponType == EWeaponType::Shield)
		{
			WeaponSocketType = EWeaponSocketType::OffHandShieldLoadoutTwo;
		}
		else
		{
			WeaponSocketType = EWeaponSocketType::OffHandWeaponLoadoutTwo;
		}
		break;
	default:
		break;
	}

	Server_SpawnAtLocation(NewEquippingCharacter->GetActorLocation());
	Server_SetCanInteract(false);
	FName AttachSocketName = NAME_None;

	FWeaponLoadout ActiveLoadout = NewEquippingCharacter->GetEquipmentComponent()->GetActiveWeaponLoadout();
	ECombatState CombatState = NewEquippingCharacter->GetCombatState();
	
	if (CombatState != ECombatState::Sheathed && ActiveLoadout.MainHandWeapon == this)
	{
		AttachSocketName = FName("MainHand");
	}
	else if (CombatState != ECombatState::Sheathed && ActiveLoadout.OffHandWeapon == this)
	{
		AttachSocketName = FName("OffHand");
	}
	else {
		AttachSocketName = NewEquippingCharacter->GetNameForWeaponSocket(WeaponSocketType);
	}

	FVector SocketPositionAdjustment = FVector::ZeroVector;
	if (CombatState != ECombatState::Sheathed && (ActiveLoadout.MainHandWeapon == this || ActiveLoadout.OffHandWeapon == this))
	{
		FVector* VectorPtr = UnsheathedSocketPositionAdjustments.Find(WeaponSocketType);
		if (VectorPtr)
		{
			SocketPositionAdjustment = *VectorPtr;
		}
	}
	else
	{
		FVector* VectorPtr = SheathedSocketPositionAdjustments.Find(WeaponSocketType);
		if (VectorPtr)
		{
			SocketPositionAdjustment = *VectorPtr;
		}
	}

	FRotator SocketRotationAdjustment = FRotator::ZeroRotator;
	if (CombatState != ECombatState::Sheathed && (ActiveLoadout.MainHandWeapon == this || ActiveLoadout.OffHandWeapon == this))
	{
		FRotator* RotatorPtr = UnsheathedSocketRotationAdjustments.Find(WeaponSocketType);
		if (RotatorPtr)
		{
			SocketRotationAdjustment = *RotatorPtr;
		}
	}
	else
	{
		FRotator* RotatorPtr = SheathedSocketRotationAdjustments.Find(WeaponSocketType);
		if (RotatorPtr)
		{
			SocketRotationAdjustment = *RotatorPtr;
		}
	}

	NewEquippingCharacter->Server_AttachActorToSocket(this, AttachSocketName, SocketPositionAdjustment, SocketRotationAdjustment);
}

void AWeapon::MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);
	GetRootMeshComponent()->SetSimulatePhysics(false);

	// Disable interactable trace collision on meshes so they don't block interactable tracing from camera
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionResponseToChannel(TRACE_INTERACTABLE, ECR_Ignore);
		}
	}
}

void AWeapon::ServerOnUnequip_Implementation()
{
	EquippingCharacter->Server_DetachActor(this);
	Super::ServerOnUnequip_Implementation();
}

void AWeapon::MulticastOnUnequip_Implementation()
{
	Super::MulticastOnUnequip_Implementation();
	GetRootMeshComponent()->SetSimulatePhysics(true);

	// Reenable interactable trace collision on meshes
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionResponseToChannel(TRACE_INTERACTABLE, ECR_Block);
		}
	}

	// Reinitialize item tooltip
	UInteractTooltipWidget* InteractTooltip = Cast<UInteractTooltipWidget>(WidgetComponent->GetUserWidgetObject());
	if (InteractTooltip)
	{
		InteractTooltip->SetInteractable(this);
		UItemTooltipWidget* ItemTooltip = Cast<UItemTooltipWidget>(InteractTooltip->GetItemTooltip());
		if (ItemTooltip)
		{
			ItemTooltip->SetItem(this);
			ItemTooltip->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

