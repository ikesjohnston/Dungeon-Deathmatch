// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Functionable.generated.h"

class ADungeonCharacter;

UINTERFACE(MinimalAPI)
class UFunctionable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IFunctionable
{
	GENERATED_IINTERFACE_BODY()

public:

	virtual void NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter);

	virtual void NativeOnEndPrimaryFunction(ADungeonCharacter* FunctioningCharacter);

	virtual void NativeOnStartSecondaryFunction(ADungeonCharacter* FunctioningCharacter);

	virtual void NativeOnEndSecondaryFunction(ADungeonCharacter* FunctioningCharacter);

	virtual void NativeOnFocusStart();

	virtual void NativeOnFocusEnd();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Functionable")
	void OnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Functionable")
	void OnEndPrimaryFunction(ADungeonCharacter* FunctioningCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Functionable")
	void OnStartSecondaryFunction(ADungeonCharacter* FunctioningCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Functionable")
	void OnEndSecondaryFunction(ADungeonCharacter* FunctioningCharacter);
};
