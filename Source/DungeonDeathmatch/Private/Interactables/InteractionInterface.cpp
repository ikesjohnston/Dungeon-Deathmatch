// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionInterface.h"

UInteractionInterface::UInteractionInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IInteractionInterface::NativeOnInteract(ADungeonCharacter* InteractingCharacter)
{
	// Call blueprint scriptable event
	Execute_OnInteract(_getUObject(), InteractingCharacter);
}

void IInteractionInterface::NativeOnEnterInteractionVolume()
{

}

void IInteractionInterface::NativeOnExitInteractionVolume()
{

}

void IInteractionInterface::NativeOnFocused()
{

}

void IInteractionInterface::NativeOnUnfocused()
{

}
