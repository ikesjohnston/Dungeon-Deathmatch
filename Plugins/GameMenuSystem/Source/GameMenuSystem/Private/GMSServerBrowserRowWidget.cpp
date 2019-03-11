// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSServerBrowserRowWidget.h"
#include "GMSMainMenuWidget.h"

#include <TextBlock.h>
#include <Button.h>

bool UGMSServerBrowserRowWidget::Initialize()
{
	bool Result = Super::Initialize();

	if (!ensure(SelectButton != nullptr))
	{
		return false;
	}
	SelectButton->OnClicked.AddDynamic(this, &UGMSServerBrowserRowWidget::OnSelectButtonPressed);

	return Result;
}

void UGMSServerBrowserRowWidget::Setup(UGMSMainMenuWidget* Parent, uint32 Index)
{
	ParentWidget = Parent;
	ServerIndex = Index;
}

void UGMSServerBrowserRowWidget::SetSessionNameText(FText Text)
{
	SessionNameText->SetText(Text);
}

void UGMSServerBrowserRowWidget::SetHostNameText(FText Text)
{
	HostNameText->SetText(Text);
}

void UGMSServerBrowserRowWidget::SetPlayerCountText(FText Text)
{
	PlayerCountText->SetText(Text);
}

void UGMSServerBrowserRowWidget::SetLatencyText(FText Text)
{
	LatencyText->SetText(Text);
}

void UGMSServerBrowserRowWidget::OnSelectButtonPressed()
{
	if (ParentWidget)
	{
		ParentWidget->SelectServerIndex(ServerIndex);
	}
}
