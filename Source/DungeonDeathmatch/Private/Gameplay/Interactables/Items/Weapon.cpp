// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "EquipmentGlobals.h"
#include "Equippable.h"
#include "Item.h"
#include "InteractTooltipWidget.h"
#include "ItemTooltipWidget.h"
#include "DungeonGameInstance.h"
#include "WeaponTraceComponent.h"
#include "DungeonDeathmatch.h"
#include "PlayerCombatComponent.h"

#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include "EquipmentComponent.h"
#include <AbilitySystemInterface.h>
#include <AbilitySystemComponent.h>

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
	if (Owner && Owner->HasAuthority())
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
	if (Owner && Owner->HasAuthority())
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

void AWeapon::ServerOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	Super::ServerOnEquip_Implementation(InEquippingActor, EquipmentSlot);

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

	ServerSpawnAtLocation(InEquippingActor->GetActorLocation());
	ServerSetCanInteract(false);

	UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(InEquippingActor->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (EquipmentComponent)
	{
		FName AttachSocketName = NAME_None;
		FVector SocketPositionAdjustment = FVector::ZeroVector;
		FRotator SocketRotationAdjustment = FRotator::ZeroRotator;
		FWeaponLoadout ActiveLoadout = EquipmentComponent->GetActiveWeaponLoadout();
		UPlayerCombatComponent* CombatComponent = Cast<UPlayerCombatComponent>(InEquippingActor->GetComponentByClass(UPlayerCombatComponent::StaticClass()));
		if (CombatComponent)
		{
			ECombatState CombatState = CombatComponent->GetCombatState();
			if (CombatState != ECombatState::Sheathed && ActiveLoadout.MainHandWeapon == this)
			{
				AttachSocketName = FName("MainHand");
			}
			else if (CombatState != ECombatState::Sheathed && ActiveLoadout.OffHandWeapon == this)
			{
				AttachSocketName = FName("OffHand");
			}
			else {
				//		AttachSocketName = NewEquippingCharacter->GetNameForWeaponSocket(WeaponSocketType);
			}

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
		}
		EquipmentComponent->ServerAttachActorToSocket(this, AttachSocketName, SocketPositionAdjustment, SocketRotationAdjustment);
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(InEquippingActor);
	if (AbilitySystemInterface)
	{
		for (TSubclassOf<UGameplayAbility> Ability : MainHandAbilities)
		{
			AbilitySystemInterface->GetAbilitySystemComponent()->GiveAbility(Ability);
		}
		for (TSubclassOf<UGameplayAbility> Ability : MainHandAltAbilities)
		{
			AbilitySystemInterface->GetAbilitySystemComponent()->GiveAbility(Ability);
		}
		for (TSubclassOf<UGameplayAbility> Ability : OffHandAbilities)
		{
			AbilitySystemInterface->GetAbilitySystemComponent()->GiveAbility(Ability);
		}
		for (TSubclassOf<UGameplayAbility> Ability : OffHandAltAbilities)
		{
			AbilitySystemInterface->GetAbilitySystemComponent()->GiveAbility(Ability);
		}
	}
}

void AWeapon::MulticastOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot)
{
	Super::MulticastOnEquip_Implementation(InEquippingActor, EquipmentSlot);
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
}

void AWeapon::ServerOnUnequip_Implementation()
{
	UEquipmentComponent* EquipmentComponent = Cast<UEquipmentComponent>(EquippingActor->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (EquipmentComponent)
	{
		EquipmentComponent->ServerDetachActor(this);
	}
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
		ServerOnHitDetected(WeaponHitResult);
	}
}

void AWeapon::ServerOnHitDetected_Implementation(FWeaponHitResult WeaponHitResult)
{
	MulticastOnHitDetected(WeaponHitResult);
}

bool AWeapon::ServerOnHitDetected_Validate(FWeaponHitResult WeaponHitResult)
{
	return true;
}

void AWeapon::MulticastOnHitDetected_Implementation(FWeaponHitResult WeaponHitResult)
{
	AActor* HitActor = WeaponHitResult.HitResult.GetActor();
	if (!HitActorsThisSwing.Contains(HitActor))
	{
		HitActorsThisSwing.Add(HitActor);

		FVector HitDirection = WeaponHitResult.HitResult.TraceEnd - WeaponHitResult.HitResult.TraceStart;
		HitDirection.Normalize();

		HitActor->TakeDamage(100000, FDamageEvent(), EquippingActor->GetInstigatorController(), this);
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

bool AWeapon::MulticastOnHitDetected_Validate(FWeaponHitResult WeaponHitResult)
{
	return true;
}
