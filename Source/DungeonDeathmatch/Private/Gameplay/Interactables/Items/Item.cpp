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

// Sets default values
AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateMovement = true;

	Value = 1.0f;

	InteractionPromptText = FText::FromString("Pick Up");
	InventoryUseTooltipText = FText::FromString("Use");

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(TRACE_INTERACTABLE);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetVisibility(false);

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

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	SetMeshStencilValue();
	Super::BeginPlay();

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

UStaticMeshComponent* AItem::GetMeshComponent()
{
	return MeshComponent;
}

FText AItem::GetItemName()
{
	return ItemName;
}

float AItem::GetValue()
{
	return Value;
}

FText AItem::GetFlavorText()
{
	return FlavorText;
}

UTexture2D* AItem::GetIcon()
{
	return Icon;
}

FLinearColor AItem::GetQualityTierColor()
{
	float R, G, B;
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

			return FLinearColor(R, G, B);
		}
	}
	return FLinearColor();
}

FText AItem::GetInventoryUseTooltipText()
{
	return InventoryUseTooltipText;
}

bool AItem::TryInventoryUse()
{
	return false;
}

// ------------------------ BEGIN INTERACTABLE INTERFACE FUNCTIONS ------------------------
void AItem::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
{
	if (Role == ROLE_Authority)
	{
		InteractingCharacter->Server_TryPickUpItem(this);
	}
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
// ------------------------ END INTERACTABLE INTERFACE FUNCTIONS ------------------------

//UWidgetComponent* AItem::GetWidgetComponent()
//{
//	return WidgetComponent;
//}

//void AItem::SetCanInteract(bool CanInteract)
//{
//	Server_SetCanInteract(CanInteract);
//}


//bool AItem::GetCanInteract()
//{
//	return bCanInteract;
//}
//
//void AItem::Server_SetCanInteract(bool CanInteract)
//{
//	bCanInteract = CanInteract;
//}
//
//FText AItem::GetInteractionPromptText()
//{
//	return InteractionPromptText;
//}
//
//FText AItem::GetInteractableName()
//{
//	return InteractableName;
//}
//
//void AItem::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
//{
//	// Server side safety check
//	if (Role == ROLE_Authority)
//	{
//
//	}
//}
//
//void AItem::OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
//{
//	// Server side safety check
//	if (Role == ROLE_Authority)
//	{
//		NativeOnInteract(InteractingCharacter);
//	}
//}
//
//void AItem::Server_OnInteract_Implementation(ADungeonCharacter* InteractingCharacter)
//{
//	OnInteract(InteractingCharacter);
//}
//
//bool AItem::Server_OnInteract_Validate(ADungeonCharacter* InteractingCharacter)
//{
//	return true;
//}
//
//void AItem::OnFocused()
//{
//	// Add glowing outline to mesh(es). Set by the post processing object in the level. This should only happen on the client.
//	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
//	for (int i = 0; i < MeshComponents.Num(); i++)
//	{
//		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
//		if (MeshComp)
//		{
//			MeshComp->SetRenderCustomDepth(true);
//		}
//	}
//
//	WidgetComponent->SetVisibility(true);
//}
//
//void AItem::OnUnfocused()
//{
//	// Remove glowing outline from mesh(es). Set by the post processing object in the level. This should only happen on the client.
//	TArray<UActorComponent*> MeshComponents = GetComponentsByClass(UMeshComponent::StaticClass());
//	for (int i = 0; i < MeshComponents.Num(); i++)
//	{
//		UMeshComponent* MeshComp = Cast<UMeshComponent>(MeshComponents[i]);
//		if (MeshComp)
//		{
//			MeshComp->SetRenderCustomDepth(false);
//		}
//	}
//
//	WidgetComponent->SetVisibility(false);
//}

