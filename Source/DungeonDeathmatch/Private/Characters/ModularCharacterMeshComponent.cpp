// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularCharacterMeshComponent.h"
#include "DungeonCharacter.h"

#include <Components/SkeletalMeshComponent.h>

UModularCharacterMeshComponent::UModularCharacterMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwningCharacter = Cast<ADungeonCharacter>(GetOwner());
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UModularCharacterMeshComponent::UModularCharacterMeshComponent - OwningCharacter is null."))
		return;
	}

	MeshComponentHelm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHelm"));
	MeshComponentHelm->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Helm, MeshComponentHelm));

	MeshComponentHair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHair"));
	MeshComponentHair->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Hair, MeshComponentHair));

	MeshComponentHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHead"));
	MeshComponentHead->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Head, MeshComponentHead));

	MeshComponentShoulderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderLeft"));
	MeshComponentShoulderLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftShoulder, MeshComponentShoulderLeft));

	MeshComponentShoulderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentShoulderRight"));
	MeshComponentShoulderRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightShoulder, MeshComponentShoulderRight));

	MeshComponentTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentTorso"));
	MeshComponentTorso->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Torso, MeshComponentTorso));

	MeshComponentChestArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentChestArmor"));
	MeshComponentChestArmor->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::ChestArmor, MeshComponentChestArmor));

	MeshComponentHandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandLeft"));
	MeshComponentHandLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftHand, MeshComponentHandLeft));

	MeshComponentHandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentHandRight"));
	MeshComponentHandRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightHand, MeshComponentHandRight));

	MeshComponentBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentBelt"));
	MeshComponentBelt->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Waist, MeshComponentBelt));

	MeshComponentLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegs"));
	MeshComponentLegs->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::Legs, MeshComponentLegs));

	MeshComponentLegArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentLegArmor"));
	MeshComponentLegArmor->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LegArmor, MeshComponentLegArmor));

	MeshComponentFootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootLeft"));
	MeshComponentFootLeft->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::LeftFoot, MeshComponentFootLeft));

	MeshComponentFootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponentFootRight"));
	MeshComponentFootRight->SetupAttachment(OwningCharacter->GetMesh());
	MeshComponentMap.Add(TTuple<EMeshSegment, USkeletalMeshComponent*>(EMeshSegment::RightFoot, MeshComponentFootRight));
}

void UModularCharacterMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UModularCharacterMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitializeDefaultMeshSegments();
}

void UModularCharacterMeshComponent::Initialize()
{
	InitializeDefaultMeshSegments();
}

TMap<EMeshSegment, USkeletalMeshComponent*> UModularCharacterMeshComponent::GetMeshComponentMap()
{
	return MeshComponentMap;
}

void UModularCharacterMeshComponent::InitializeDefaultMeshSegments()
{
	if (!OwningCharacter) return;
	OwningCharacter->GetMesh()->SetCollisionObjectType(ECC_Pawn);
	OwningCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwningCharacter->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	OwningCharacter->GetMesh()->SetVisibility(false);

	for (TTuple<EMeshSegment, USkeletalMeshComponent*> Tuple : MeshComponentMap)
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

void UModularCharacterMeshComponent::ServerUpdateMeshSegment_Implementation(EMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	MulticastUpdateMeshSegment(MeshSegment, NewMesh);
}

bool UModularCharacterMeshComponent::ServerUpdateMeshSegment_Validate(EMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	return true;
}

void UModularCharacterMeshComponent::MulticastUpdateMeshSegment_Implementation(EMeshSegment MeshSegment, USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent** MeshComponentPtr = MeshComponentMap.Find(MeshSegment);
	if (MeshComponentPtr)
	{
		USkeletalMeshComponent* MeshComponent = *MeshComponentPtr;
		if (MeshComponent)
		{
			if (NewMesh)
			{
				MeshComponent->SetSkeletalMesh(NewMesh);
				/*if (OwningCharacter->IsLocallyControlled() && RenderCaptureActor)
				{
					RenderCaptureActor->UpdateMeshSegment(MeshSegment, NewMesh);
				}*/
			}
			else
			{
				USkeletalMesh** DefaultMeshPtr = DefaultMeshMap.Find(MeshSegment);
				if (DefaultMeshPtr)
				{
					MeshComponent->SetSkeletalMesh(*DefaultMeshPtr);
					/*if (IsLocallyControlled() && RenderCaptureActor)
					{
						RenderCaptureActor->UpdateMeshSegment(MeshSegment, *DefaultMeshPtr);
					}*/
				}
			}
		}
	}
}

