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
#include "WeaponTraceComponent.h"
#include "DungeonDeathmatch.h"

// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;
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

TArray<TSubclassOf<UDungeonGameplayAbility>> AWeapon::GetMainHandAbilities()
{
	return MainHandAbilities;
}

TArray<TSubclassOf<UDungeonGameplayAbility>> AWeapon::GetMainHandAltAbilities()
{
	return MainHandAltAbilities;
}

TArray<TSubclassOf<UDungeonGameplayAbility>> AWeapon::GetOffHandAbilities()
{
	return OffHandAbilities;
}

TArray<TSubclassOf<UDungeonGameplayAbility>> AWeapon::GetOffHandAltAbilities()
{
	return OffHandAltAbilities;
}

void AWeapon::StartSwing()
{
	if (SwingSounds.Num() > 0)
	{
		int SoundIndex = FMath::RandRange(0, SwingSounds.Num() - 1);
		USoundCue* SwingSoundToPlay = SwingSounds[SoundIndex];
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SwingSoundToPlay, GetActorLocation());
	}
	if (SwingParticles)
	{
		//SwingParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(SwingParticles, DamagingVolume, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}

	// Activate all attached hit trace components on the server
	AActor* Owner = GetOwner();
	if (Owner && Owner->Role == ROLE_Authority)
	{
		TArray<UActorComponent*> WeaponTraceComponents = GetComponentsByClass(UWeaponTraceComponent::StaticClass());
		for (UActorComponent* ActorComponent : WeaponTraceComponents)
		{
			UWeaponTraceComponent* WeaponTraceComponent = Cast<UWeaponTraceComponent>(ActorComponent);
			if (WeaponTraceComponent)
			{
				WeaponTraceComponent->SetIsHitTracing(true);
			}
		}
	}
}

void AWeapon::StopSwing()
{
	if (SwingParticleSystemComponent)
	{
		//SwingParticleSystemComponent->DestroyComponent(true);
	}

	// Deactivate all attached hit trace components on the server
	// Activate all attached hit trace components on server
	AActor* Owner = GetOwner();
	if (Owner && Owner->Role == ROLE_Authority)
	{
		TArray<UActorComponent*> WeaponTraceComponents = GetComponentsByClass(UWeaponTraceComponent::StaticClass());
		for (UActorComponent* ActorComponent : WeaponTraceComponents)
		{
			UWeaponTraceComponent* WeaponTraceComponent = Cast<UWeaponTraceComponent>(ActorComponent);
			if (WeaponTraceComponent)
			{
				WeaponTraceComponent->SetIsHitTracing(false);
			}
		}
	}

	HitActorsThisSwing.Empty();
	DamagedActorsThisSwing.Empty();
	BlockingActorsThisSwing.Empty();
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
			MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
		}
	}

	for (TSubclassOf<UGameplayAbility> Ability : MainHandAbilities)
	{
		NewEquippingCharacter->GiveAbility(Ability);
	}
	for (TSubclassOf<UGameplayAbility> Ability : MainHandAltAbilities)
	{
		NewEquippingCharacter->GiveAbility(Ability);
	}
	for (TSubclassOf<UGameplayAbility> Ability : OffHandAbilities)
	{
		NewEquippingCharacter->GiveAbility(Ability);
	}
	for (TSubclassOf<UGameplayAbility> Ability : OffHandAltAbilities)
	{
		NewEquippingCharacter->GiveAbility(Ability);
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
			MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
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

void AWeapon::OnHitDetected(FWeaponHitResult WeaponHitResult)
{
	if (Role == ROLE_Authority)
	{
		Server_OnHitDetected(WeaponHitResult);
	}
}

void AWeapon::Server_OnHitDetected_Implementation(FWeaponHitResult WeaponHitResult)
{
	Multicast_OnHitDetected(WeaponHitResult);
}

bool AWeapon::Server_OnHitDetected_Validate(FWeaponHitResult WeaponHitResult)
{
	return true;
}

void AWeapon::Multicast_OnHitDetected_Implementation(FWeaponHitResult WeaponHitResult)
{
	AActor* HitActor = WeaponHitResult.HitResult.GetActor();
	if (!HitActorsThisSwing.Contains(HitActor))
	{
		HitActorsThisSwing.Add(HitActor);

		FVector HitDirection = WeaponHitResult.HitResult.TraceEnd - WeaponHitResult.HitResult.TraceStart;
		HitDirection.Normalize();

		HitActor->TakeDamage(100000, FDamageEvent(), EquippingCharacter->GetController(), this);
		UPrimitiveComponent* HitComponent = WeaponHitResult.HitResult.GetComponent();
		if (HitComponent)
		{
			HitComponent->AddForceAtLocation(HitDirection * -10000, WeaponHitResult.HitResult.ImpactPoint);
		}

		UMeshComponent* MeshComponent = Cast<UMeshComponent>(WeaponHitResult.HitResult.GetComponent());
		if (MeshComponent)
		{
			EPhysicalSurface PhysicalSurface = WeaponHitResult.HitResult.PhysMaterial->SurfaceType;
			USoundCue* SoundToPlay = nullptr;
			int SoundIndex = 0;
			switch (PhysicalSurface)
			{
			case PHYSICAL_SURFACE_METAL:
				SoundIndex = FMath::RandRange(0, MetalHitSounds.Num() - 1);
				SoundToPlay = MetalHitSounds[SoundIndex];
				break;
			case PHYSICAL_SURFACE_STONE:
				SoundIndex = FMath::RandRange(0, StoneHitSounds.Num() - 1);
				SoundToPlay = StoneHitSounds[SoundIndex];
				break;
			case PHYSICAL_SURFACE_WOOD:
				SoundIndex = FMath::RandRange(0, WoodHitSounds.Num() - 1);
				SoundToPlay = WoodHitSounds[SoundIndex];
				break;
			case PHYSICAL_SURFACE_LEATHER:
				SoundIndex = FMath::RandRange(0, LeatherHitSounds.Num() - 1);
				SoundToPlay = LeatherHitSounds[SoundIndex];
				break;
			case PHYSICAL_SURFACE_CLOTH:
				SoundIndex = FMath::RandRange(0, ClothHitSounds.Num() - 1);
				SoundToPlay = ClothHitSounds[SoundIndex];
				break;
			case PHYSICAL_SURFACE_FLESH:
				SoundIndex = FMath::RandRange(0, FleshHitSounds.Num() - 1);
				SoundToPlay = FleshHitSounds[SoundIndex];
				break;
			default:
				break;
			}
			if (SoundToPlay)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, WeaponHitResult.HitResult.ImpactPoint);
			}
			if (HitParticles)
			{
				FRotator ParticleRotation = FVector::CrossProduct(WeaponHitResult.HitResult.ImpactNormal, HitDirection).Rotation();
				FTransform EmitterTransform = FTransform(ParticleRotation, WeaponHitResult.HitResult.ImpactPoint, FVector::OneVector);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, EmitterTransform);
			}
		}

		switch (WeaponHitResult.WeaponTraceType)
		{
		case EWeaponTraceType::Edge:
			break;
		case EWeaponTraceType::Center:
			break;
		case EWeaponTraceType::Base:
			break;
		default:
			break;
		}
	}
}

bool AWeapon::Multicast_OnHitDetected_Validate(FWeaponHitResult WeaponHitResult)
{
	return true;
}
