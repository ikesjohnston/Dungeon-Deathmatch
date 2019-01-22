// Fill out your copyright notice in the Description page of Project Settings.

#include "DragAndDropItemWidget.h"
#include "Item.h"
#include "DungeonGameInstance.h"
#include <Image.h>
#include <WidgetLayoutLibrary.h>

UDragAndDropItemWidget::UDragAndDropItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UDragAndDropItemWidget::Initialize()
{
	bool Result = Super::Initialize();

	ItemImage->SetVisibility(ESlateVisibility::Collapsed);

	return Result;
}

void UDragAndDropItemWidget::StartDragAndDropOperation(AItem* Item)
{
	ItemImage->SetBrushFromTexture(Item->GetIcon());
	ItemImage->SetBrushSize(Item->GetGridSizeVector());
	ItemImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UDragAndDropItemWidget::StopDragAndDropOperation()
{
	ItemImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UDragAndDropItemWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	// Render the drag and drop image over the cursor
	SetRenderTranslation(UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()));

	Super::NativeTick(MyGeometry, DeltaTime);
}