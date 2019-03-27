// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularHumanoidMeshComponent.h"
#include "RenderCaptureComponent.h"

#include <GameFramework/Character.h>
#include <GameFramework/Actor.h>
#include <Components/SkeletalMeshComponent.h>

UModularHumanoidMeshComponent::UModularHumanoidMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UModularHumanoidMeshComponent::UModularHumanoidMeshComponent - OwningCharacter is null."))
		return;
	}

	MeshComponentHelm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHelm"));
	MeshComponentHelm->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Helm, MeshComponentHelm));

	MeshComponentHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHair"));
	MeshComponentHair->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Hair, MeshComponentHair));

	MeshComponentHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHead"));
	MeshComponentHead->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Head, MeshComponentHead));

	MeshComponentShoulderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderLeft"));
	MeshComponentShoulderLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftShoulder, MeshComponentShoulderLeft));

	MeshComponentShoulderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderRight"));
	MeshComponentShoulderRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightShoulder, MeshComponentShoulderRight));

	MeshComponentTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentTorso"));
	MeshComponentTorso->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Torso, MeshComponentTorso));

	MeshComponentChestArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentChestArmor"));
	MeshComponentChestArmor->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::ChestArmor, MeshComponentChestArmor));

	MeshComponentHandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandLeft"));
	MeshComponentHandLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftHand, MeshComponentHandLeft));

	MeshComponentHandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandRight"));
	MeshComponentHandRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightHand, MeshComponentHandRight));

	MeshComponentBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentBelt"));
	MeshComponentBelt->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Waist, MeshComponentBelt));

	MeshComponentLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegs"));
	MeshComponentLegs->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::Legs, MeshComponentLegs));

	MeshComponentLegArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegArmor"));
	MeshComponentLegArmor->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LegArmor, MeshComponentLegArmor));

	MeshComponentFootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootLeft"));
	MeshComponentFootLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::LeftFoot, MeshComponentFootLeft));

	MeshComponentFootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootRight"));
	MeshComponentFootRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*>(EHumanoidMeshSegment::RightFoot, MeshComponentFootRight));
}

void UModularHumanoidMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UModularHumanoidMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitializeDefaultMeshSegments();
}

void UModularHumanoidMeshComponent::Initialize()
{
	InitializeDefaultMeshSegments();
}

void UModularHumanoidMeshComponent::InitializeDefaultMeshSegments()
{
	if (!OwningCharacter) return;
	OwningCharacter->GetMesh()->SetCollisionObjectType(ECC_Pawn);
	OwningCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwningCharacter->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	OwningCharacter->GetMesh()->SetVisibility(false);

	for (TTuple<EHumanoidMeshSegment, USkeletalMeshComponent*> Tuple : MeshComponentMap)
	{
		USkeletalMeshComponent* MeshComp = Tuple.Value;
		MeshComp->SetSkeletalMesh(nullptr);

		USkeletalMesh** SkeletalMeshPtr = DefaultMeshMap.Find(Tuple.Key);
		if (SkeletalMeshPtr)
		{
			USkeletalMesh* SkeletalMesh = *SkeletalMeshPtr;
			if (SkeletalMesh)
			{
				MeshComp->SetSkeletalMesh(SkeletalMesh);
			}
		}

		MeshComp->SetCollisionObjectType(ECC_Pawn);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetMasterPoseComponent(OwningCharacter->GetMesh());
	}
}

void UModularHumanoidMeshComponent::ServerUpdateMeshSegment_Implementation(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	MulticastUpdateMeshSegment(MeshSegment, NewMesh);
}

bool UModularHumanoidMeshComponent::ServerUpdateMeshSegment_Validate(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	return true;
}

void UModularHumanoidMeshComponent::MulticastUpdateMeshSegment_Implementation(EHumanoidMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent** MeshComponentPtr = MeshComponentMap.Find(MeshSegment);
	if (MeshComponentPtr)
	{
		USkeletalMeshComponent* MeshComponent = *MeshComponentPtr;
		if (MeshComponent)
		{
			if (!NewMesh)
			{
				USkeletalMesh** DefaultMeshPtr = DefaultMeshMap.Find(MeshSegment);
				if (DefaultMeshPtr)
				{
					NewMesh = *DefaultMeshPtr;
				}
			}
			MeshComponent->SetSkeletalMesh(NewMesh);

			URenderCaptureComponent* RenderCaptureComponent = Cast<URenderCaptureComponent>(OwningCharacter->GetComponentByClass(URenderCaptureComponent::StaticClass()));
			if (RenderCaptureComponent)
			{
				RenderCaptureComponent->UpdateMeshSegment(MeshSegment, NewMesh);
			}
		}
	}
}

