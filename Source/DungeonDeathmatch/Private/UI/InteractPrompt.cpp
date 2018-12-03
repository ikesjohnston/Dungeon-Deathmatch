// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractPrompt.h"
#include "Interactable.h"

#define LOCTEXT_NAMESPACE "Interactions" 

void UInteractPrompt::OnInteractableFocused(AInteractable* Interactable)
{
	FFormatNamedArguments Args;
	Args.Add("Prompt", Interactable->GetInteractionPromptText());
	Args.Add("Name", Interactable->GetInteractableName());
	InteractionText = FText::Format(LOCTEXT("InteractionText", "{Prompt} {Name}"), Args);

	SetVisibility(ESlateVisibility::Visible);
}

void UInteractPrompt::OnInteractableUnfocused()
{
	InteractionText = FText::GetEmpty();
	SetVisibility(ESlateVisibility::Hidden);
}

#undef LOCTEXT_NAMESPACE 
