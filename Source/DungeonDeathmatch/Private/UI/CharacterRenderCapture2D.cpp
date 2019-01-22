// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterRenderCapture2D.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Components/SkeletalMeshComponent.h>
#include "DungeonCharacter.h"
#include "Armor.h"

// Sets default values
ACharacterRenderCapture2D::ACharacterRenderCapture2D()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Initialize all mesh segments
	MeshComponentMaster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentMaster"));
	MeshComponentMaster->SetupAttachment(RootComponent);

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

	InputRotationRate = 180.0f;
}

// Called when the game starts or when spawned
void ACharacterRenderCapture2D::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterRenderCapture2D::OnArmorEquipped(AArmor* Armor)
{
	if (Armor)
	{
		UpdateMeshSegments(Armor->GetArmorMeshMap());
	}
}

void ACharacterRenderCapture2D::UpdateMeshSegments(TMap<EMeshSegment, USkeletalMesh*> MeshMap)
{
	for (TPair<EMeshSegment, USkeletalMesh*> MeshPair : MeshMap)
	{
		EMeshSegment MeshSegment = MeshPair.Key;
		USkeletalMesh* Mesh = MeshPair.Value;
		
		USkeletalMeshComponent* MeshComponent = *MeshComponentMap.Find(MeshSegment);
		if (MeshComponent)
		{
			MeshComponent->SetSkeletalMesh(Mesh);
		}
	}
}

// Called every frame
void ACharacterRenderCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterRenderCapture2D::InitializeCharacter(ADungeonCharacter* Character)
{
	Character->OnArmorEquipped.RemoveDynamic(this, &ACharacterRenderCapture2D::OnArmorEquipped);
	DisplayedCharacter = Character;
	DisplayedCharacter->OnArmorEquipped.AddDynamic(this, &ACharacterRenderCapture2D::OnArmorEquipped);

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

void ACharacterRenderCapture2D::RotateCharacter(float RotationInput, float DeltaTime)
{
	FRotator CurrentRotation = MeshComponentMaster->GetComponentRotation();
	MeshComponentMaster->AddRelativeRotation(FQuat(FVector(0, 0, 1), InputRotationRate * RotationInput * DeltaTime));
}
