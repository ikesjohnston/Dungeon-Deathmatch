// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterRenderCapture2D.h"
#include "ModularHumanoidMeshComponent.h"

#include <Components/SceneCaptureComponent2D.h>
#include <Components/SkeletalMeshComponent.h>
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
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Helm, MeshComponentHelm));

	MeshComponentHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHair"));
	MeshComponentHair->SetupAttachment(MeshComponentMaster);
	MeshComponentHair->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Hair, MeshComponentHair));

	MeshComponentHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHead"));
	MeshComponentHead->SetupAttachment(MeshComponentMaster);
	MeshComponentHead->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Head, MeshComponentHead));

	MeshComponentShoulderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderLeft"));
	MeshComponentShoulderLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentShoulderLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftShoulder, MeshComponentShoulderLeft));

	MeshComponentShoulderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderRight"));
	MeshComponentShoulderRight->SetupAttachment(MeshComponentMaster);
	MeshComponentShoulderRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightShoulder, MeshComponentShoulderRight));

	MeshComponentTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentTorso"));
	MeshComponentTorso->SetupAttachment(MeshComponentMaster);
	MeshComponentTorso->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Torso, MeshComponentTorso));

	MeshComponentChestArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentChestArmor"));
	MeshComponentChestArmor->SetupAttachment(MeshComponentMaster);
	MeshComponentChestArmor->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::ChestArmor, MeshComponentChestArmor));

	MeshComponentHandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandLeft"));
	MeshComponentHandLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentHandLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftHand, MeshComponentHandLeft));

	MeshComponentHandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandRight"));
	MeshComponentHandRight->SetupAttachment(MeshComponentMaster);
	MeshComponentHandRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightHand, MeshComponentHandRight));

	MeshComponentBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentBelt"));
	MeshComponentBelt->SetupAttachment(MeshComponentMaster);
	MeshComponentBelt->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Waist, MeshComponentBelt));

	MeshComponentLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegs"));
	MeshComponentLegs->SetupAttachment(MeshComponentMaster);
	MeshComponentLegs->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Legs, MeshComponentLegs));

	MeshComponentLegArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegArmor"));
	MeshComponentLegArmor->SetupAttachment(MeshComponentMaster);
	MeshComponentLegArmor->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LegArmor, MeshComponentLegArmor));

	MeshComponentFootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootLeft"));
	MeshComponentFootLeft->SetupAttachment(MeshComponentMaster);
	MeshComponentFootLeft->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftFoot, MeshComponentFootLeft));

	MeshComponentFootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootRight"));
	MeshComponentFootRight->SetupAttachment(MeshComponentMaster);
	MeshComponentFootRight->SetMasterPoseComponent(MeshComponentMaster);
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightFoot, MeshComponentFootRight));

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

void ACharacterRenderCapture2D::UpdateMeshSegment(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh)
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

void ACharacterRenderCapture2D::InitializeRender(AActor* CapturedActor)
{
	UModularHumanoidMeshComponent* ModularMeshComponent = Cast<UModularHumanoidMeshComponent>(CapturedActor->GetComponentByClass(UModularHumanoidMeshComponent::StaticClass()));
	if (ModularMeshComponent)
	{
		TMap<EHumanoidMeshSegment, USkeletalMeshComponent*> CharacterMeshComponentMap = ModularMeshComponent->GetMeshComponentMap();
		for (TPair<EHumanoidMeshSegment, USkeletalMeshComponent*> MeshComponentPair : CharacterMeshComponentMap)
		{
			EHumanoidMeshSegment MeshSegment = MeshComponentPair.Key;
			USkeletalMeshComponent* CharacterMeshComponent = MeshComponentPair.Value;

			USkeletalMeshComponent* MeshComponent = *MeshComponentMap.Find(MeshSegment);
			if (MeshComponent)
			{
				MeshComponent->SetSkeletalMesh(CharacterMeshComponent->SkeletalMesh);
			}
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
