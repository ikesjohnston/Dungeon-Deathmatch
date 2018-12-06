// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include <Components/MeshComponent.h>
#include <WidgetComponent.h>
#include "InventoryComponent.h"
#include "DungeonCharacter.h"
#include "EquipmentComponent.h" 
#include "InteractTooltip.h"
#include "ItemTooltip.h"
#include "DungeonGameMode.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weight = 1.0f;
	Value = 1.0f;

	InteractionPromptText = FText::FromString("Pick Up");
}

AItem::~AItem()
{

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	SetQualityTierStencilValue();
	Super::BeginPlay();

	// Initialize item tooltip
	UInteractTooltip* InteractTooltip = Cast<UInteractTooltip>(WidgetComponent->GetUserWidgetObject());
	if (InteractTooltip)
	{
		UItemTooltip* ItemTooltip = Cast<UItemTooltip>(InteractTooltip->GetItemTooltip());
		if (ItemTooltip)
		{
			ItemTooltip->SetItem(this);
			ItemTooltip->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AItem::SetQualityTierStencilValue()
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
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FText AItem::GetItemName()
{
	return ItemName;
}

float AItem::GetWeight()
{
	return Weight;
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
	ADungeonGameMode* GameMode = Cast<ADungeonGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		FLinearColor* Color = GameMode->GetItemQualityTierColors().Find(QualityTier);
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

void AItem::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	Super::NativeOnInteract(InteractingCharacter);
}

