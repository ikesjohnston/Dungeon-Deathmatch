// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerBrowserRowWidget.h"
#include <TextBlock.h>
#include <Button.h>
#include "MainMenuWidget.h"

bool UServerBrowserRowWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(SelectButton != nullptr))
	{
		return false;
	}
	SelectButton->OnClicked.AddDynamic(this, &UServerBrowserRowWidget::OnSelectButtonPressed);

	return Result;
}

void UServerBrowserRowWidget::Setup(UMainMenuWidget* Parent, uint32 Index)
{
	ParentWidget = Parent;
	ServerIndex = Index;
}

void UServerBrowserRowWidget::SetSessionNameText(FText Text)
{
	SessionNameText->SetText(Text);
}

void UServerBrowserRowWidget::SetHostNameText(FText Text)
{
	HostNameText->SetText(Text);
}

void UServerBrowserRowWidget::SetPlayerCountText(FText Text)
{
	PlayerCountText->SetText(Text);
}

void UServerBrowserRowWidget::SetLatencyText(FText Text)
{
	LatencyText->SetText(Text);
}

void UServerBrowserRowWidget::OnSelectButtonPressed()
{
	if (ParentWidget)
	{
		ParentWidget->SelectServerIndex(ServerIndex);
	}
}
