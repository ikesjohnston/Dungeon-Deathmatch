// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractTooltip.h"
#include "Interactable.h"
#include "ItemTooltip.h"
#include <UserWidget.h>

#define LOCTEXT_NAMESPACE "Interactions" 

void UInteractTooltip::SetInteractable(AInteractable* NewInteractable)
{
	Interactable = NewInteractable;

	FFormatNamedArguments Args;
	Args.Add("Prompt", Interactable->GetInteractionPromptText());
	Args.Add("Name", Interactable->GetInteractableName());
	InteractionText = FText::Format(LOCTEXT("InteractionText", "{Prompt} {Name}"), Args);

	ItemTooltipWidgetName = "WBP_ItemTooltip";

	InitItemTooltip();
}

UItemTooltip* UInteractTooltip::GetItemTooltip()
{
	return ItemTooltip;
}

void UInteractTooltip::OnInteractableFocused()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UInteractTooltip::OnInteractableUnfocused()
{
	InteractionText = FText::GetEmpty();
	SetVisibility(ESlateVisibility::Hidden);
}

void UInteractTooltip::InitItemTooltip()
{
	bool TooltipFound = false;
	if (!ItemTooltipWidgetName.IsNone())
	{
		ItemTooltip = Cast<UItemTooltip>(WidgetTree->FindWidget(ItemTooltipWidgetName));
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
