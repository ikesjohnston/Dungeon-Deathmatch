// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentSlotWidget.h"
#include "DraggableItemWidget.h"
#include "DungeonGameInstance.h"
#include "DungeonPlayerController.h"
#include "Armor.h"
#include "Weapon.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "DungeonHUD.h"

#include <CanvasPanel.h>
#include <WidgetBlueprintLibrary.h>
#include <CanvasPanelSlot.h>
#include <Image.h>

UEquipmentSlotWidget::UEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BindingRetryTime = 0.1f;
	BorderSize = 2.0f;
}

bool UEquipmentSlotWidget::Initialize()
{
	bool Result = Super::Initialize();

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

	// Initialize the slot size
	float GridSlotSize = 40.0f;

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GridSlotSize = GameInstance->GetInventoryGridSlotSize();
	}

	// Resize slot based on type
	FInventoryGridPair* EquipmentSizePtr = EquipmentSlotSizes.Find(SlotType);
	if (EquipmentSizePtr)
	{
		FInventoryGridPair SlotGridSize = *EquipmentSizePtr;
		FVector2D SlotSize = FVector2D(GridSlotSize * SlotGridSize.Column, GridSlotSize * SlotGridSize.Row);
		SlotBackground->SetBrushSize(SlotSize);
		SlotBorder->SetBrushSize(SlotSize + FVector2D(BorderSize, BorderSize));
		// Temporary highlighting method, will be using a dedicated texture, but for now just using a basic translucent square highlight
		SlotHighlight->SetBrushSize(SlotSize);
	}

	ResetDefaultImage();

	BindToController();

	return Result;
}

void UEquipmentSlotWidget::BindToController()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Controller)
	{
		BindToSource(Controller->GetPawn());
		Controller->OnBeginItemDrag.AddDynamic(this, &UEquipmentSlotWidget::OnBeginItemDrag);
		Controller->OnEndItemDrag.AddDynamic(this, &UEquipmentSlotWidget::OnEndItemDrag);
		bIsBoundToController = true;
	}
	if (!bIsBoundToController)
	{
		GetWorld()->GetTimerManager().SetTimer(BindToControllerTimerHandle, this, &UEquipmentSlotWidget::BindToController, BindingRetryTime, false);
	}
}

void UEquipmentSlotWidget::BindToSource(AActor* Source)
{
	if (SourceEquipmentComponent)
	{
		SourceEquipmentComponent->OnItemEquipped.RemoveDynamic(this, &UEquipmentSlotWidget::OnItemEquipped);
		SourceEquipmentComponent->OnItemUnequipped.RemoveDynamic(this, &UEquipmentSlotWidget::OnItemUnequipped);
	}

	SourceEquipmentComponent = Cast<UEquipmentComponent>(Source->GetComponentByClass(UEquipmentComponent::StaticClass()));
	if (SourceEquipmentComponent)
	{
		SourceEquipmentComponent->OnItemEquipped.AddDynamic(this, &UEquipmentSlotWidget::OnItemEquipped);
		SourceEquipmentComponent->OnItemUnequipped.AddDynamic(this, &UEquipmentSlotWidget::OnItemUnequipped);
	}

	SourceInventoryComponent = Cast<UInventoryComponent>(Source->GetComponentByClass(UInventoryComponent::StaticClass()));
}

void UEquipmentSlotWidget::ResetDefaultImage()
{
	float GridSlotSize = 40.0f;

	UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GridSlotSize = GameInstance->GetInventoryGridSlotSize();
	}

	// Set default item texture
	UTexture2D** TexturePtr = EquipmentSlotTextures.Find(SlotType);
	if (TexturePtr)
	{
		UTexture2D* EmptyEquipmentSlotTexture = *TexturePtr;
		DefaultEquipmentImage->SetBrushFromTexture(EmptyEquipmentSlotTexture);
		DefaultEquipmentImage->SetColorAndOpacity(FLinearColor::White);
	}

	FInventoryGridPair* EquipmentSizePtr = EquipmentTextureSizes.Find(SlotType);
	if (EquipmentSizePtr)
	{
		FInventoryGridPair SlotTextureSize = *EquipmentSizePtr;
		FVector2D TextureSize = FVector2D(GridSlotSize * SlotTextureSize.Column, GridSlotSize * SlotTextureSize.Row);
		DefaultEquipmentImage->SetBrushSize(TextureSize);
	}
}

bool UEquipmentSlotWidget::GetCanFitDraggedItem()
{
	return bCanFitDraggedItem;
}

void UEquipmentSlotWidget::ProcessItemDragAndDrop()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		if (SourceEquipmentComponent)
		{
			UDraggableItemWidget* DraggedItemWidget = Controller->GetDraggedItem();
			UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
			UDraggableItemWidget* ClickedItemWidget = Controller->GetClickedItem();
			if (DraggedItemWidget && bCanFitDraggedItem) {
				AEquippable* ItemToEquip = Cast<AEquippable>(DraggedItemWidget->GetItem());
				if (SelectedItemWidget)
				{
					// Replacing equipment
					AEquippable* ItemToUnequip = Cast<AEquippable>(SelectedItemWidget->GetItem());
					if (ItemToEquip && ItemToUnequip)
					{
						Controller->StopDraggingItem(false);
						SelectedItemWidget->StartDragging();
						SourceEquipmentComponent->ServerUnequipItem(ItemToUnequip, SlotType);
						SourceEquipmentComponent->ServerEquipItemToSlot(ItemToEquip, SlotType);
						UGameplayStatics::PlaySound2D(GetWorld(), ItemToEquip->GetInteractionSound());
						Controller->SetSelectedItem(EquippedItemWidget);
					}
				}
				else
				{
					// Equipping a new item
					SourceEquipmentComponent->ServerEquipItemToSlot(ItemToEquip, SlotType);
					UGameplayStatics::PlaySound2D(GetWorld(), ItemToEquip->GetInteractionSound());
					Controller->StopDraggingItem(false);
					Controller->SetSelectedItem(nullptr);
					if (SlotHighlight)
					{
						SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
					}
				}
			}
			else if (ClickedItemWidget && !DraggedItemWidget)
			{
				// Unequipping an item
				AEquippable* ItemToUnequip = Cast<AEquippable>(ClickedItemWidget->GetItem());
				if (ItemToUnequip)
				{
					SourceEquipmentComponent->ServerUnequipItem(ItemToUnequip, SlotType);
					ClickedItemWidget->StartDragging();
					UGameplayStatics::PlaySound2D(GetWorld(), BeginDragSound);
				}
			}
		}
	}
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
			if (SlotType == EEquipmentSlot::WeaponLoadoutOneMainHand || SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoMainHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EWeaponHand::TwoHand:
			if (SlotType == EEquipmentSlot::WeaponLoadoutOneMainHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoMainHand)
			{
				bCanFitDraggedItem = true;
			}
			break;
		case EWeaponHand::OffHand:
			if (SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand)
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
	if (SlotBorder)
	{
		SlotBorder->SetColorAndOpacity(DefaultBorderColor);
	}
}

void UEquipmentSlotWidget::OnItemEquipped(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (Slot == SlotType)
	{
		// Add the drag and drop widget on top of the inventory grid
		UDungeonGameInstance* GameInstance = Cast<UDungeonGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			FString DragAndDropString = FString("Drag&Drop_");
			DragAndDropString.Append(Equippable->GetItemName().ToString());
			FName DragAndDropName = FName(*DragAndDropString);

			UDraggableItemWidget* DraggableWidget = Cast<UDraggableItemWidget>(CreateWidget(GetOwningPlayer(), GameInstance->GetDragAndDropItemWidgetClass(), DragAndDropName));
			if (DraggableWidget)
			{
				FInventoryGridPair* GridSizePtr = EquipmentSlotSizes.Find(SlotType);
				if (GridSizePtr)
				{
					FInventoryGridPair SlotGridSize = *GridSizePtr;
					DraggableWidget->InitializeDraggableEquipment(Equippable, SlotGridSize);
					if (DraggableEquipmentCanvas)
					{
						DraggableEquipmentCanvas->AddChild(DraggableWidget);
						UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DraggableWidget->Slot);
						CanvasSlot->SetAutoSize(true);
						DraggableWidget->SetRenderTranslation(FVector2D(BorderSize / 2, BorderSize / 2));
					}
					DraggableWidget->SetVisibility(ESlateVisibility::Visible);
				}
				EquippedItemWidget = DraggableWidget;
			}
			DefaultEquipmentImage->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlotWidget::OnItemEquipped - Failed to get game instance."));
		}
	}
	else
	{
		// If a two handed weapon was equipped and this is the offhand slot, update the slot item image to signify the slot is locked by the two hander
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (Weapon && Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
		{
			if ((Slot == EEquipmentSlot::WeaponLoadoutOneMainHand && SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand) || (Slot == EEquipmentSlot::WeaponLoadoutTwoMainHand && SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand))
			{
				DefaultEquipmentImage->SetBrushFromTexture(Weapon->GetIcon());
				DefaultEquipmentImage->SetBrushSize(Weapon->GetGridSizeVector());
				DefaultEquipmentImage->SetColorAndOpacity(LockedSlotItemColor);
				DefaultEquipmentImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
	}
}

void UEquipmentSlotWidget::OnItemUnequipped(AEquippable* Equippable, EEquipmentSlot Slot)
{
	if (Slot == SlotType)
	{
		EquippedItemWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (DraggableEquipmentCanvas)
		{
			DraggableEquipmentCanvas->RemoveChild(EquippedItemWidget);
		}
		EquippedItemWidget = nullptr;

		ResetDefaultImage();
		DefaultEquipmentImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		// If a two handed weapon was unequipped and this is the offhand slot, restore the default item image
		AWeapon* Weapon = Cast<AWeapon>(Equippable);
		if (Weapon && Weapon->GetWeaponHand() == EWeaponHand::TwoHand)
		{
			if ((Slot == EEquipmentSlot::WeaponLoadoutOneMainHand && SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand) || (Slot == EEquipmentSlot::WeaponLoadoutTwoMainHand && SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand))
			{
				ResetDefaultImage();
			}
		}
	}
}

void UEquipmentSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Get the equipment type and determine if it can go in this slot. If so, highlight the slot.
	UDraggableItemWidget* DraggedItem = nullptr;
	bCanFitDraggedItem = false;
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Controller)
	{
		DraggedItem = Controller->GetDraggedItem();
		if (DraggedItem)
		{
			AItem* Item = DraggedItem->GetItem();
			if (Item)
			{
				AArmor* Armor = Cast<AArmor>(Item);
				AWeapon* Weapon = Cast<AWeapon>(Item);
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
				else if (Weapon)
				{
					switch (Weapon->GetWeaponHand())
					{
					case EWeaponHand::OneHand:
						if (SlotType == EEquipmentSlot::WeaponLoadoutOneMainHand || SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoMainHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand)
						{
							bCanFitDraggedItem = true;
						}
						break;
					case EWeaponHand::TwoHand:
						if (SlotType == EEquipmentSlot::WeaponLoadoutOneMainHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoMainHand)
						{
							bCanFitDraggedItem = true;
						}
						break;
					case EWeaponHand::OffHand:
						if (SlotType == EEquipmentSlot::WeaponLoadoutOneOffHand || SlotType == EEquipmentSlot::WeaponLoadoutTwoOffHand)
						{
							bCanFitDraggedItem = true;
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}

	if (DraggedItem && SlotHighlight)
	{
		SlotHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (bCanFitDraggedItem)
		{
			SlotHighlight->SetColorAndOpacity(ValidOverlapHighlightColor);
		}
		else
		{
			SlotHighlight->SetColorAndOpacity(InvalidOverlapHighlightColor);
		}
	}
}

void UEquipmentSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (SlotHighlight)
	{
		SlotHighlight->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FReply UEquipmentSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		UDraggableItemWidget* SelectedItem = Controller->GetSelectedItem();
		if (SelectedItem)
		{
			if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				if (SelectedItem)
				{
					Controller->SetClickedItem(SelectedItem);
					return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
				}
			}
			else if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
			{
				if (SourceEquipmentComponent)
				{
					UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
					// Unequip item and drop it
					AEquippable* ItemToDrop = Cast<AEquippable>(SelectedItemWidget->GetItem());
					if (ItemToDrop)
					{
						SourceEquipmentComponent->ServerUnequipItem(ItemToDrop, SlotType);
						if (SourceInventoryComponent)
						{
							SourceInventoryComponent->ServerRequestDropItem(ItemToDrop, false);
						}
						UGameplayStatics::PlaySound2D(GetWorld(), ItemToDrop->GetInteractionSound());
						Controller->SetSelectedItem(nullptr);
						ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
						if (HUD)
						{
							HUD->HideTooltip();
						}
					}
				}
			}
			else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
			{
				if (SourceEquipmentComponent)
				{
					UDraggableItemWidget* SelectedItemWidget = Controller->GetSelectedItem();
					// Unequip item and attempt to move it to the character's inventory
					AEquippable* ItemToUnequip = Cast<AEquippable>(SelectedItemWidget->GetItem());
					if (ItemToUnequip)
					{
						SourceEquipmentComponent->ServerUnequipItem(ItemToUnequip, SlotType, true);
						UGameplayStatics::PlaySound2D(GetWorld(), ItemToUnequip->GetInteractionSound());
						Controller->SetSelectedItem(nullptr);
						ADungeonHUD* HUD = Cast<ADungeonHUD>(Controller->GetHUD());
						if (HUD)
						{
							HUD->HideTooltip();
						}
					}
				}
			}
		}
	}

	return FReply::Handled();
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			ProcessItemDragAndDrop();
			Controller->SetClickedItem(nullptr);
			Controller->SetSelectedRenderCaptureActor(nullptr);
		}
	}

	return FReply::Handled();
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		UDraggableItemWidget* ClickedItem = Controller->GetClickedItem();
		UDraggableItemWidget* DraggedItem = Controller->GetDraggedItem();
		if (ClickedItem && !DraggedItem)
		{
			Controller->SetSelectedItem(nullptr);
			ProcessItemDragAndDrop();
			Controller->SetClickedItem(nullptr);
		}
	}
}
