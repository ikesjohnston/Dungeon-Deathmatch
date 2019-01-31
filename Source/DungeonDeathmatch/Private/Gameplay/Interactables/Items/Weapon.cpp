// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include "EquipmentGlobals.h"
#include "Equippable.h"

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
	FName AttachSocketName = NewEquippingCharacter->GetNameForWeaponSocket(WeaponSocketType);

	FVector SocketPositionAdjustment = FVector::ZeroVector;
	FVector* VectorPtr = UnsheathedSocketPositionAdjustments.Find(WeaponSocketType);
	if (VectorPtr)
	{
		SocketPositionAdjustment = *VectorPtr;
	}

	FRotator SocketRotationAdjustment = FRotator::ZeroRotator;
	FRotator* RotatorPtr = UnsheathedSocketRotationAdjustments.Find(WeaponSocketType);
	if (RotatorPtr)
	{
		SocketRotationAdjustment = *RotatorPtr;
	}

	NewEquippingCharacter->Server_AttachActorToSocket(this, AttachSocketName, SocketPositionAdjustment, SocketRotationAdjustment);
}

void AWeapon::MulticastOnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(NewEquippingCharacter, EquipmentSlot);
	GetRootMeshComponent()->SetSimulatePhysics(false);
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
}

//void AWeapon::OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter)
//{
//	Super::OnEquip(NewEquippingCharacter);
//
//	/*UMeshComponent* RootMeshComponent = GetRootMeshComponent();
//	if (RootMeshComponent)
//	{
//		RootMeshComponent->SetSimulatePhysics(false);
//		RootMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		RootMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
//	}*/
//}
//
//void AWeapon::OnUnequip_Implementation()
//{
//	//if (Role == ROLE_Authority)
//	//{
//	//	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//	//	bool bWasReturnedToInventory = EquippingCharacter->TryAddItemToInventory(this);
//	//	// If the item wasn't able to be picked up, just drop it
//	//	if (!bWasReturnedToInventory)
//	//	{
//	//		GetMeshComponent()->SetSimulatePhysics(true);
//	//		GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	//		GetMeshComponent()->SetCollisionObjectType(TRACE_INTERACTABLE);
//	//		GetMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
//	//	}
//	//	Super::OnUnequip();
//	//}
//}

