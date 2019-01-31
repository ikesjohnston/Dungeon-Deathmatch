// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryMenuWidget.h"
#include "DungeonPlayerController.h"
#include "DungeonCharacter.h"
#include "InteractiveCharacterRenderWidget.h"

UInventoryMenuWidget::UInventoryMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BindingRetryTime = 0.1f;
}

bool UInventoryMenuWidget::Initialize()
{
	bool Result = Super::Initialize();

	// Map equipment slots
	EquipmentSlots.Add(EEquipmentSlot::Head, EquipmentSlotHead);
	EquipmentSlots.Add(EEquipmentSlot::Neck, EquipmentSlotNeck);
	EquipmentSlots.Add(EEquipmentSlot::Shoulders, EquipmentSlotShoulders);
	EquipmentSlots.Add(EEquipmentSlot::Chest, EquipmentSlotChest);
	EquipmentSlots.Add(EEquipmentSlot::Hands, EquipmentSlotHands);
	EquipmentSlots.Add(EEquipmentSlot::FingerOne, EquipmentSlotFingerOne);
	EquipmentSlots.Add(EEquipmentSlot::FingerTwo, EquipmentSlotFingerTwo);
	EquipmentSlots.Add(EEquipmentSlot::Waist, EquipmentSlotWaist);
	EquipmentSlots.Add(EEquipmentSlot::Legs, EquipmentSlotLegs);
	EquipmentSlots.Add(EEquipmentSlot::Feet, EquipmentSlotFeet);
	EquipmentSlots.Add(EEquipmentSlot::WeaponLoadoutOneMainHand, EquipmentSlotLoadoutOneWeaponMainHand);
	EquipmentSlots.Add(EEquipmentSlot::WeaponLoadoutOneOffHand, EquipmentSlotLoadoutOneWeaponOffHand);
	EquipmentSlots.Add(EEquipmentSlot::WeaponLoadoutTwoMainHand, EquipmentSlotLoadoutTwoWeaponMainHand);
	EquipmentSlots.Add(EEquipmentSlot::WeaponLoadoutTwoOffHand, EquipmentSlotLoadoutTwoWeaponOffHand);

	BindToController();

	return Result;
}

void UInventoryMenuWidget::BindToController()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller)
	{
		ADungeonCharacter* Character = Cast<ADungeonCharacter>(Controller->GetPawn());
		if (Character)
		{
			if (InteractiveCharacterRender)
			{
				ACharacterRenderCapture2D* RenderCaptureActor = Character->GetRenderCaptureActor();
				if (RenderCaptureActor)
				{
					InteractiveCharacterRender->SetRenderCaptureActor(RenderCaptureActor);
					bIsSlotBound = true;
				}
			}
		}
	}

	if (!bIsSlotBound)
	{
		GetWorld()->GetTimerManager().SetTimer(BindSlotTimerHandle, this, &UInventoryMenuWidget::BindToController, BindingRetryTime, false);
	}
}