// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractiveCharacterRenderWidget.h"
#include "DungeonPlayerController.h"
#include "DungeonCharacter.h"

UInteractiveCharacterRenderWidget::UInteractiveCharacterRenderWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UInteractiveCharacterRenderWidget::Initialize()
{
	bool Result = Super::Initialize();

	return true;
}

void UInteractiveCharacterRenderWidget::SetRenderCaptureActor(ACharacterRenderCapture2D* NewRenderCaptureActor)
{
	RenderCaptureActor = NewRenderCaptureActor;
}

FReply UInteractiveCharacterRenderWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			ADungeonCharacter* Character = Cast<ADungeonCharacter>(Controller->GetPawn());
			if (Character)
			{
				RenderCaptureActor = Character->GetRenderCaptureActor();
				Controller->SetSelectedRenderCaptureActor(RenderCaptureActor);
			}
		}
	}

	return FReply::Handled();
}

FReply UInteractiveCharacterRenderWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		ADungeonPlayerController* Controller = Cast<ADungeonPlayerController>(GetOwningPlayer());
		if (Controller)
		{
			Controller->SetSelectedRenderCaptureActor(nullptr);
		}
	}

	return FReply::Handled();
}
