// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCursorWidget.h"
#include "DungeonPlayerController.h"
#include <Image.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>

UDungeonCursorWidget::UDungeonCursorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BindingRetryTime = 0.1f;
}

bool UDungeonCursorWidget::Initialize()
{
	bool Result = Super::Initialize();

	ResetCursorProperties();

	// Only try binding when the game is running
	BindToController();

	return Result;
}

void UDungeonCursorWidget::Hide()
{
	if (CursorImage)
	{
		CursorImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDungeonCursorWidget::Show()
{
	ResetCursorProperties();
}

void UDungeonCursorWidget::BindToController()
{
	ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Controller && !Controller->GetCursor())
	{
		Controller->SetCursor(this);
		bIsCursorBound = true;
	}

	if (!bIsCursorBound)
	{
		GetWorld()->GetTimerManager().SetTimer(BindCursorTimerHandle, this, &UDungeonCursorWidget::BindToController, BindingRetryTime, false);
	}
}

void UDungeonCursorWidget::ResetCursorProperties()
{
	if (CursorImage)
	{
		CursorImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		CursorImage->SetBrushFromTexture(DefaultCursorTexture);
		CursorImage->SetBrushSize(DefaultCursorSize);
		CursorImage->SetRenderTranslation(DefaultCursorSize * .5);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDungeonCursorWidget::ResetCursorPropertiese - No CursorImage widget found for %s."), *GetName());
	}
}
