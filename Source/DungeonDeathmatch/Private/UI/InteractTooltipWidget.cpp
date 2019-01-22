// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractTooltipWidget.h"
#include "Interactable.h"
#include "ItemTooltipWidget.h"
#include <UserWidget.h>
#include "Item.h"
#include <WidgetTree.h>

#define LOCTEXT_NAMESPACE "Interactions" 

void UInteractTooltipWidget::SetInteractable(AActor* NewInteractable)
{
	Interactable = NewInteractable;

	IInteractable* InteractableInterface = Cast<IInteractable>(Interactable);
	if (InteractableInterface)
	{
		FFormatNamedArguments Args;
		Args.Add("Prompt", InteractableInterface->Execute_GetInteractionPromptText(Interactable));
		Args.Add("Name", InteractableInterface->Execute_GetInteractableName(Interactable));
		InteractionText = FText::Format(LOCTEXT("InteractionText", "{Prompt} {Name}"), Args);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractTooltip::SetInteractable - %s does not inherit from a class that implements IInteractable."), *GetName());
	}

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

			AItem* Item = Cast<AItem>(Interactable);
			if (Item)
			{
				ItemTooltip->SetItem(Item);
			}
		}
	}
	if (!TooltipFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractTooltip::InitItemTooltip - No ItemTooltip widget found in %s. Verify that ItemTooltipWidgetName is correctly set."), *GetName());
	}
}

#undef LOCTEXT_NAMESPACE 
