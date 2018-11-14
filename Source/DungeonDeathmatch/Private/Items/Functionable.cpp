// Fill out your copyright notice in the Description page of Project Settings.

#include "Functionable.h"

UFunctionable::UFunctionable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IFunctionable::NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	// Call blueprint scriptable event
	Execute_OnStartPrimaryFunction(_getUObject(), FunctioningCharacter);
}

void IFunctionable::NativeOnEndPrimaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	// Call blueprint scriptable event
	Execute_OnEndPrimaryFunction(_getUObject(), FunctioningCharacter);
}

void IFunctionable::NativeOnStartSecondaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	Execute_OnStartSecondaryFunction(_getUObject(), FunctioningCharacter);
}

void IFunctionable::NativeOnEndSecondaryFunction(ADungeonCharacter* FunctioningCharacter)
{
	Execute_OnEndSecondaryFunction(_getUObject(), FunctioningCharacter);
}

void IFunctionable::NativeOnFocusStart()
{

}

void IFunctionable::NativeOnFocusEnd()
{

}
