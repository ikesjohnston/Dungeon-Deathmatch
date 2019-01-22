// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentSlotWidget.h"
#include "DraggableItemWidget.h"
#include "DungeonGameInstance.h"
#include <Image.h>
#include "DungeonPlayerController.h"
#include "Armor.h"
#include "Weapon.h"

UEquipmentSlotWidget::UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BindingRetryTime = 0.1f;
	BorderSize = 2.0f;
}

bool UEquipmentSlotWidget::Initialize()
{
	bool Result = Super::Initialize();

	UpdateEquipment();

	if (SlotBackground)
	{
		SlotBackground->SetColorAndOpacity(DefaultBackgroundColor);
	}
	if (SlotHighlight)
	{
		SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SlotBorder)
	{
		SlotBorder->SetColorAndOpacity(DefaultBorderColor);
	}

	BindToController();

	return Result;
}

void UEquipmentSlotWidget::BindToController()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		Controller->OnBeginItemDrag.AddDynamic(this, &UEquipmentSlotWidget::OnBeginItemDrag);
		Controller->OnEndItemDrag.AddDynamic(this, &UEquipmentSlotWidget::OnEndItemDrag);
		bIsSlotBound = true;
	}

	if (!bIsSlotBound)
	{
		GetWorld()->GetTimerManager().SetTimer(BindSlotTimerHandle, this, &UEquipmentSlotWidget::BindToController, BindingRetryTime, false);
	}
}

void UEquipmentSlotWidget::UpdateEquipment()
{
	if (!EquippedItemWidget || !DefaultEquipmentImage || !SlotBackground || !SlotHighlight || !SlotBorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlotWidget::UpdateEquipment - Essential widgets missing from %s. Verify that widgets are correctly set."), *GetName());

		return;
	}

	AItem* EquippedItem = EquippedItemWidget->GetItem();
	if (!EquippedItem)
	{
		DefaultEquipmentImage->SetVisibility(ESlateVisibility::Visible);
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);

		float GridSlotSize = 40.0f;

		UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			float GridSlotSize = GameInstance->GetInventoryGridSlotSize();
		}

		// Set default item texture
		UTexture2D** TexturePtr = EquipmentSlotTextures.Find(SlotType);
		if (TexturePtr)
		{
			UTexture2D* EmptyEquipmentSlotTexture = *TexturePtr;
			DefaultEquipmentImage->SetBrushFromTexture(EmptyEquipmentSlotTexture);
		}

		// Resize slot based on type
		FInventoryGridPair* GridSizePtr = EquipmentSlotSizes.Find(SlotType);
		if (GridSizePtr)
		{
			FInventoryGridPair SlotGridSize = *GridSizePtr;
			FVector2D SlotSize = FVector2D(GridSlotSize * SlotGridSize.Column, GridSlotSize * SlotGridSize.Row);
			SlotBackground->SetBrushSize(SlotSize);
			SlotBorder->SetBrushSize(SlotSize + FVector2D(BorderSize, BorderSize));
			// Temporary highlighting method, will be using a dedicated texture, but for now just using a basic translucent square highlight
			SlotHighlight->SetBrushSize(SlotSize);
		}
		GridSizePtr = EquipmentTextureSizes.Find(SlotType);
		if (GridSizePtr)
		{
			FInventoryGridPair SlotTextureSize = *GridSizePtr;
			FVector2D TextureSize = FVector2D(GridSlotSize * SlotTextureSize.Column, GridSlotSize * SlotTextureSize.Row);
			DefaultEquipmentImage->SetBrushSize(TextureSize);
		}
	}
	else
	{
		DefaultEquipmentImage->SetVisibility(ESlateVisibility::Collapsed);
		EquippedItemWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

bool UEquipmentSlotWidget::GetCanFitDraggedItem()
{
	return bCanFitDraggedItem;
}

void UEquipmentSlotWidget::OnBeginItemDrag(AItem* Item)
{
	// Get the equipment type and determine if it can go in this slot. If so, highlight the slot.
	bCanFitDraggedItem = false;
	AArmor* Armor = Cast<AArmor>(Item);
	if (Armor)
	{
		switch (Armor->GetArmorSlot())
		{
		case EArmorSlot::Head:
			if (SlotType == EEquipmentSlot::Head)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Neck:
			if (SlotType == EEquipmentSlot::Neck)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Shoulders:
			if (SlotType == EEquipmentSlot::Shoulders)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Chest:
			if (SlotType == EEquipmentSlot::Chest)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Hands:
			if (SlotType == EEquipmentSlot::Hands)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Finger:
			if (SlotType == EEquipmentSlot::FingerOne || SlotType == EEquipmentSlot::FingerTwo)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Waist:
			if (SlotType == EEquipmentSlot::Waist)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Legs:
			if (SlotType == EEquipmentSlot::Legs)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EArmorSlot::Feet:
			if (SlotType == EEquipmentSlot::Feet)
			{
				bCanFitDraggedItem = true;
			}
			break;
		default:
			break;
		}
	}

	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		switch (Weapon->GetWeaponHand())
		{
		case EWeaponHand::OneHand:
			if (SlotType == EEquipmentSlot::WeaponMainHand || SlotType == EEquipmentSlot::WeaponOffHand)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EWeaponHand::TwoHand:
			if (SlotType == EEquipmentSlot::WeaponMainHand)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EWeaponHand::OffHand:
			if (SlotType == EEquipmentSlot::WeaponOffHand)
			{
				bCanFitDraggedItem = true;
			}
			break;
		default:
			break;
		}
	}

	if (bCanFitDraggedItem && SlotBorder)
	{
		SlotBorder->SetColorAndOpacity(HighlightColor);
	}
}

void UEquipmentSlotWidget::OnEndItemDrag(AItem* Item)
{
	bCanFitDraggedItem = false;
	if (bCanFitDraggedItem && SlotBorder)
	{
		SlotBorder->SetColorAndOpacity(DefaultBorderColor);
	}
}
