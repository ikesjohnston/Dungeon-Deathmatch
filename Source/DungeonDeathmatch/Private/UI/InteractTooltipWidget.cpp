// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractTooltipWidget.h"
#include "Interactable.h"
#include "ItemTooltipWidget.h"
#include <UserWidget.h>

#define LOCTEXT_NAMESPACE "Interactions" 

void UInteractTooltipWidget::SetInteractable(AInteractable* NewInteractable)
{
	Interactable = NewInteractable;

	FFormatNamedArguments Args;
	Args.Add("Prompt", Interactable->GetInteractionPromptText());
	Args.Add("Name", Interactable->GetInteractableName());
	InteractionText = FText::Format(LOCTEXT("InteractionText", "{Prompt} {Name}"), Args);

	ItemTooltipWidgetName = "WBP_ItemTooltip";

	InitItemTooltip();
}

UItemTooltipWidget* UInteractTooltipWidget::GetItemTooltip()
{
	return ItemTooltip;
}

void UInteractTooltipWidget::OnInteractableFocused()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UInteractTooltipWidget::OnInteractableUnfocused()
{
	InteractionText = FText::GetEmpty();
	SetVisibility(ESlateVisibility::Hidden);
}

void UInteractTooltipWidget::InitItemTooltip()
{
	bool TooltipFound = false;
	if (!ItemTooltipWidgetName.IsNone())
	{
		ItemTooltip = Cast<UItemTooltipWidget>(WidgetTree->FindWidget(ItemTooltipWidgetName));
		if (ItemTooltip)
		{
			ItemTooltip->SetVisibility(ESlateVisibility::Hidden);
			TooltipFound = true;
		}
	}
	if (!TooltipFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractTooltip::InitItemTooltip - No ItemTooltip widget found in %s. Verify that ItemTooltipWidgetName is correctly set."), *GetName());
	}
}

#undef LOCTEXT_NAMESPACE 
