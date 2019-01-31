// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterRenderCapture2D.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Components/SkeletalMeshComponent.h>
#include "DungeonCharacter.h"
#include "Armor.h"
#include <PhysicsEngine/PhysicsConstraintComponent.h>
#include <Kismet/KismetRenderingLibrary.h>

// Sets default values
ACharacterRenderCapture2D::ACharacterRenderCapture2D()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMeshComponent"));
	RootMeshComponent->SetVisibility(false);
	RootMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootMeshComponent->SetupAttachment(RootComponent);

	// Initialize all mesh segments
	MeshComponentMaster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentMaster"));
	MeshComponentMaster->SetupAttachment(RootMeshComponent);

	MeshComponentHelm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHelm"));
	MeshComponentHelm->SetupAttachment(MeshComponentMaster);
	MeshComponentHelm->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Helm, MeshComponentHelm));

	MeshComponentHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHair"));
	MeshComponentHair->SetupAttachment(MeshComponentMaster);
	MeshComponentHair->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Hair, MeshComponentHair));

	MeshComponentHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHead"));
	MeshComponentHead->SetupAttachment(MeshComponentMaster);
	MeshComponentHead->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Head, MeshComponentHead));

	MeshComponentShoulderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderLeft"));
	MeshComponentShoulderLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentShoulderLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftShoulder, MeshComponentShoulderLeft));

	MeshComponentShoulderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderRight"));
	MeshComponentShoulderRight->SetupAttachment(MeshComponentMaster);
	MeshComponentShoulderRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightShoulder, MeshComponentShoulderRight));

	MeshComponentTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentTorso"));
	MeshComponentTorso->SetupAttachment(MeshComponentMaster);
	MeshComponentTorso->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Torso, MeshComponentTorso));

	MeshComponentChestArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentChestArmor"));
	MeshComponentChestArmor->SetupAttachment(MeshComponentMaster);
	MeshComponentChestArmor->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::ChestArmor, MeshComponentChestArmor));

	MeshComponentHandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandLeft"));
	MeshComponentHandLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentHandLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftHand, MeshComponentHandLeft));

	MeshComponentHandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandRight"));
	MeshComponentHandRight->SetupAttachment(MeshComponentMaster);
	MeshComponentHandRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightHand, MeshComponentHandRight));

	MeshComponentBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentBelt"));
	MeshComponentBelt->SetupAttachment(MeshComponentMaster);
	MeshComponentBelt->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Waist, MeshComponentBelt));

	MeshComponentLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegs"));
	MeshComponentLegs->SetupAttachment(MeshComponentMaster);
	MeshComponentLegs->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Legs, MeshComponentLegs));

	MeshComponentLegArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegArmor"));
	MeshComponentLegArmor->SetupAttachment(MeshComponentMaster);
	MeshComponentLegArmor->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LegArmor, MeshComponentLegArmor));

	MeshComponentFootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootLeft"));
	MeshComponentFootLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentFootLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftFoot, MeshComponentFootLeft));

	MeshComponentFootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootRight"));
	MeshComponentFootRight->SetupAttachment(MeshComponentMaster);
	MeshComponentFootRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightFoot, MeshComponentFootRight));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(CameraComponent);

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComponent->SetupAttachment(RootComponent);

	RenderTargetTextureWidth = 2048;
	RenderTargetTextureHeight = 2048;

	RotationTarget = 0;
	RotationTargetLerpAlpha = .05;
}

// Called when the game starts or when spawned
void ACharacterRenderCapture2D::BeginPlay()
{
	Super::BeginPlay();
	
	RenderTargetTexture = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), RenderTargetTextureWidth, RenderTargetTextureHeight);

	SceneCaptureComponent->TextureTarget = RenderTargetTexture;
	SceneCaptureComponent->ShowOnlyActors.Add(this);
}

// Called every frame
void ACharacterRenderCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentTurntableYaw = RootMeshComponent->GetRelativeTransform().GetRotation().Rotator().Yaw;

	float NewYaw = FMath::Lerp(CurrentTurntableYaw, RotationTarget, RotationTargetLerpAlpha);
	if (NewYaw < -180)
	{
		NewYaw += 360;
		RotationTarget += 360;
	}
	else if (NewYaw > 180)
	{
		NewYaw -= 360;
		RotationTarget -= 360;
	}

	FRotator NewRotation = FRotator(0, NewYaw, 0);
	NewRotation.Normalize();
	RootMeshComponent->SetRelativeRotation(NewRotation);
}

void ACharacterRenderCapture2D::UpdateMeshSegment(EMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent** MeshComponentPtr = MeshComponentMap.Find(MeshSegment);
	if (MeshComponentPtr)
	{
		USkeletalMeshComponent* MeshComponent = *MeshComponentPtr;
		if (MeshComponent)
		{
			MeshComponent->SetSkeletalMesh(NewMesh);
		}
	}
}

void ACharacterRenderCapture2D::AttachActorToSocket(AActor* Actor, FName SocketName, FVector RelativePosition, FRotator RelativeRotation)
{
	ENetRole ActorRole = Actor->GetLocalRole();
	Actor->Role = ROLE_Authority;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Template = Actor;

	UClass* ActorClass = Actor->GetClass();
	AActor* ActorToAttach = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnParameters);

	Actor->Role = ActorRole;

	AItem* Item = Cast<AItem>(ActorToAttach);
	if (Item)
	{
		UMeshComponent* RootMesh = Item->GetRootMeshComponent();
		if (RootMesh)
		{
			RootMesh->SetSimulatePhysics(false);
		}

		Item->Execute_OnUnfocused(Item);
	}

	DuplicateAttachedActorMap.Add(TTuple<AActor*, AActor*>(Actor, ActorToAttach));

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	AttachmentRules.bWeldSimulatedBodies = true;

	ActorToAttach->AttachToComponent(MeshComponentMaster, AttachmentRules, SocketName);

	ActorToAttach->SetActorRelativeLocation(RelativePosition);
	ActorToAttach->SetActorRelativeRotation(RelativeRotation);

	SceneCaptureComponent->ShowOnlyActors.Add(ActorToAttach);
}

void ACharacterRenderCapture2D::DetachActor(AActor* Actor)
{
	AActor** ActorPtr = DuplicateAttachedActorMap.Find(Actor);
	if (ActorPtr)
	{
		AActor* ActorToDetach = *ActorPtr;
		if (ActorToDetach)
		{
			ActorToDetach->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			SceneCaptureComponent->ShowOnlyActors.Remove(ActorToDetach);
			ActorToDetach->Destroy();
		}
	}
}

UTextureRenderTarget2D* ACharacterRenderCapture2D::GetRenderTargetTexture()
{
	return RenderTargetTexture;
}

void ACharacterRenderCapture2D::InitializeCharacter(ADungeonCharacter* Character)
{
	TMap<EMeshSegment, USkeletalMeshComponent*> CharacterMeshComponentMap = Character->GetMeshComponentMap();
	for (TPair<EMeshSegment, USkeletalMeshComponent*> MeshComponentPair : CharacterMeshComponentMap)
	{
		EMeshSegment MeshSegment = MeshComponentPair.Key;
		USkeletalMeshComponent* CharacterMeshComponent = MeshComponentPair.Value;

		USkeletalMeshComponent* MeshComponent = *MeshComponentMap.Find(MeshSegment);
		if (MeshComponent)
		{
			MeshComponent->SetSkeletalMesh(CharacterMeshComponent->SkeletalMesh);
		}
	}
}

void ACharacterRenderCapture2D::AddToTargetRotation(float Rotation)
{
	RotationTarget += Rotation;
}

void ACharacterRenderCapture2D::StopRotating()
{
	RotationTarget = RootMeshComponent->GetRelativeTransform().GetRotation().Rotator().Yaw;
}
