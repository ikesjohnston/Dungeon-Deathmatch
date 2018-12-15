// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryEquipmentMenuWidget.h"
#include "InventoryEquipmentSlotWidget.h"
#include <GridPanel.h>
#include <UserWidget.h>
#include <GridSlot.h>

//void UInventoryEquipmentMenuWidget::InitializeGrid()
//{
//	bool GridFound = false;
//	if (!InventoryGridPanelName.IsNone())
//	{
//		UGridPanel* InventoryGrid = Cast<UGridPanel>(WidgetTree->FindWidget(InventoryGridPanelName));
//		if (InventoryGrid)
//		{
//			GridFound = true;
//			for (int SlotIndex = 0; SlotIndex < NumberOfInventorySlots; SlotIndex++)
//			{
//				FString SlotString = FString("InventorySlot");
//				SlotString.AppendInt(SlotIndex);
//				FName SlotName = FName(*SlotString);
//
//				UInventoryEquipmentSlot* InventorySlot = Cast<UInventoryEquipmentSlot>(CreateWidget(GetOwningPlayer(), UInventoryEquipmentSlot::StaticClass(), SlotName));
//				if (InventorySlot)
//				{
//					InventorySlots.Add(InventorySlot);
//					UGridSlot* GridSlot = InventoryGrid->AddChildToGrid(InventorySlot);
//					int SlotRow = SlotIndex / NumberOfGridColumns;
//					int SlotColumn = SlotIndex % NumberOfGridColumns;
//					GridSlot->SetRow(SlotRow);
//					GridSlot->SetColumn(SlotColumn);
//					GridSlot->SetPadding(FMargin(2, 2, 2, 2));
//				}
//			}
//		}
//	}
//	if (GridFound)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("UInventoryMenu::InitializeGrid - No InventoryGrid widget found in %s. Verify that InventoryGridPanelName is correctly set."), *GetName());
//	}
//}
