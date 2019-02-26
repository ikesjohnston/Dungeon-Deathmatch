// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerBrowserLineWidget.h"
#include <TextBlock.h>
#include <Button.h>
#include "MainMenuWidget.h"

bool UServerBrowserLineWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(SelectButton != nullptr))
	{
		return false;
	}
	SelectButton->OnClicked.AddDynamic(this, &UServerBrowserLineWidget::OnSelectButtonPressed);

	return Result;
}

void UServerBrowserLineWidget::Setup(UMainMenuWidget* Parent, uint32 Index)
{
	ParentWidget = Parent;
	ServerIndex = Index;
}

void UServerBrowserLineWidget::SetSessionNameText(FText Text)
{
	SessionNameText->SetText(Text);
}

void UServerBrowserLineWidget::SetHostNameText(FText Text)
{
	HostNameText->SetText(Text);
}

void UServerBrowserLineWidget::SetPlayerCountText(FText Text)
{
	PlayerCountText->SetText(Text);
}

void UServerBrowserLineWidget::SetLatencyText(FText Text)
{
	LatencyText->SetText(Text);
}

void UServerBrowserLineWidget::OnSelectButtonPressed()
{
	if (ParentWidget)
	{
		ParentWidget->SelectServerIndex(ServerIndex);
	}
}
