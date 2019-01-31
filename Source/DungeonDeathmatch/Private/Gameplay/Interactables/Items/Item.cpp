// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include <Components/MeshComponent.h>
#include <WidgetComponent.h>
#include "InventoryComponent.h"
#include "DungeonCharacter.h"
#include "EquipmentComponent.h" 
#include "InteractTooltipWidget.h"
#include "ItemTooltipWidget.h"
#include "DungeonGameInstance.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateMovement = true;

	InteractionPromptText = FText::FromString("Pick Up");
	InventoryUseTooltipText = FText::FromString("Use");

	RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMeshComponent"));
	RootMeshComponent->SetSimulatePhysics(true);
	RootComponent = RootMeshComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetVisibility(false);
	WidgetComponent->SetupAttachment(RootMeshComponent);
	WidgetComponent->SetRelativeLocation(FVector(10, 0, 10));

	bCanInteract = true;
}

AItem::~AItem()
{

}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, bCanInteract);
}

void AItem::PreInitializeComponents()
{
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetCollisionObjectType(TRACE_INTERACTABLE);
			MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		}
	}
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	SetMeshStencilValue();

	Super::BeginPlay();

	// Make sure item grid size wasn't accidentally set to 0. Can't clamp pair values for blueprint because it is also used for grid coordinates.
	if (GridSize.Row == 0)
	{
		GridSize.Row = 1;
	}
	if (GridSize.Column == 0)
	{
		GridSize.Column = 1;
	}

	// Initialize item tooltip
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


void AItem::SetMeshStencilValue()
{
	switch (QualityTier)
	{
	case EItemQualityTier::Normal:
		QualityTierStencilValue = STENCIL_ITEM_DEFAULT;
		break;
	case EItemQualityTier::Uncommon:
		QualityTierStencilValue = STENCIL_ITEM_UNCOMMON;
		break;
	case EItemQualityTier::Rare:
		QualityTierStencilValue = STENCIL_ITEM_RARE;
		break;
	case EItemQualityTier::Epic:
		QualityTierStencilValue = STENCIL_ITEM_EPIC;
		break;
	case EItemQualityTier::Legendary:
		QualityTierStencilValue = STENCIL_ITEM_LEGENDARY;
		break;
	default:
		break;
	}

	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCustomDepthStencilValue(QualityTierStencilValue);
		}
	}
}

UStaticMeshComponent* AItem::GetRootMeshComponent()
{
	return RootMeshComponent;
}

FText AItem::GetItemName()
{
	return ItemName;
}

FInventoryGridPair AItem::GetGridSize()
{
	return GridSize;
}

FVector2D AItem::GetGridSizeVector()
{
	FVector2D ZeroVector = FVector2D(0, 0);
	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		float GridSlotSize = GameInstance->GetInventoryGridSlotSize();
		FVector2D SizeVector = FVector2D(GridSlotSize * GridSize.Column, GridSlotSize * GridSize.Row);
		return SizeVector;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::GetGridSizeVector - Failed to get game instance."));
	}

	return ZeroVector;
}

FText AItem::GetFlavorText()
{
	return FlavorText;
}

UTexture2D* AItem::GetIcon()
{
	return Icon;
}

USoundCue* AItem::GetInteractionSound()
{
	return InteractionSound;
}

FLinearColor AItem::GetQualityTierColor()
{
	float R, G, B, A;
	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		FLinearColor* Color = GameInstance->GetItemQualityTierColors().Find(QualityTier);
		if (Color)
		{
			// There is a bug with FLinearColors that have mid to high R values being set to 0? Need to manually set for now.
			if (QualityTier == EItemQualityTier::Legendary)
			{
				R = 1.0f;
			}
			else
			{
				R = Color->R;
			}

			G = Color->G;
			B = Color->B;
			A = Color->A;

			return FLinearColor(R, G, B, A);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::GetQualityTierColor - Failed to get game instance."));
	}
	return FLinearColor();
}

FLinearColor AItem::GetQualityTierTextColor()
{
	float R, G, B, A;
	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		FLinearColor* Color = GameInstance->GetItemQualityTierTextColors().Find(QualityTier);
		if (Color)
		{
			// There is a bug with FLinearColors that have mid to high R values being set to 0? Need to manually set for now.
			if (QualityTier == EItemQualityTier::Legendary)
			{
				R = 1.0f;
			}
			else
			{
				R = Color->R;
			}

			G = Color->G;
			B = Color->B;
			A = Color->A;

			return FLinearColor(R, G, B, A);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::GetQualityTierTextColor - Failed to get game instance."));
	}
	return FLinearColor();
}

FText AItem::GetInventoryUseTooltipText()
{
	return InventoryUseTooltipText;
}

// ------------------------ BEGIN INTERACTABLE INTERFACE FUNCTIONS ------------------------
void AItem::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	InteractingCharacter->Server_RequestPickUpItem(this);
}

void AItem::OnFocused_Implementation()
{
	// Add glowing outline to mesh(es). Set by the post processing object in the level. This should only happen on the client.
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(true);
		}
	}

	WidgetComponent->SetVisibility(true);
}

void AItem::OnUnfocused_Implementation()
{
	// Remove glowing outline from mesh(es). Set by the post processing object in the level. This should only happen on the client.
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetRenderCustomDepth(false);
		}
	}

	WidgetComponent->SetVisibility(false);
}

bool AItem::GetCanInteract_Implementation()
{
	return bCanInteract;
}

void AItem::SetCanInteract_Implementation(bool CanInteract)
{
	bCanInteract = CanInteract;
}

FText AItem::GetInteractionPromptText_Implementation()
{
	return InteractionPromptText;
}

FText AItem::GetInteractableName_Implementation()
{
	return ItemName;
}

void AItem::Server_SetCanInteract_Implementation(bool CanInteract)
{
	Execute_SetCanInteract(this, CanInteract);
}

bool AItem::Server_SetCanInteract_Validate(bool CanInteract)
{
	return true;
}

void AItem::Server_Despawn_Implementation()
{
	Server_SetCanInteract(false);
	Multicast_Despawn();
}

bool AItem::Server_Despawn_Validate()
{
	return true;
}

void AItem::Multicast_Despawn_Implementation()
{
	WidgetComponent->SetVisibility(false);

	// Disable physics on root mesh only
	UMeshComponent* RootMeshComponent = GetRootMeshComponent();
	if (RootMeshComponent)
	{
		RootMeshComponent->SetSimulatePhysics(false);
	}
	
	// Disable collision and rendering on all mesh components
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComp->SetVisibility(false);
		}
	}

	// Move item to origin
	SetActorLocation(FVector::ZeroVector);

	Execute_OnUnfocused(this);
}

void AItem::Server_SpawnAtLocation_Implementation(const FVector Location, const FVector EjectionForce /*= FVector(0, 0, 0)*/)
{
	Multicast_SpawnAtLocation(Location, EjectionForce);
	Server_SetCanInteract(true);
}

bool AItem::Server_SpawnAtLocation_Validate(const FVector Location, const FVector EjectionForce /*= FVector(0, 0, 0)*/)
{
	return true;
}

void AItem::Multicast_SpawnAtLocation_Implementation(const FVector Location, const FVector EjectionForce /*= FVector(0, 0, 0)*/)
{
	// Enable collision and rendering on all mesh components
	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
	for (int i = 0; i < MeshComponents.Num(); i++)
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetVisibility(true);
		}
	}

	// Enable physics on root mesh only and add any ejection force to the root
	UMeshComponent* RootMeshComponent = GetRootMeshComponent();
	if (RootMeshComponent)
	{
		RootMeshComponent->SetSimulatePhysics(true);
		RootMeshComponent->AddForce(EjectionForce);
	}

	WidgetComponent->SetVisibility(false);

	SetActorLocation(Location);
}

