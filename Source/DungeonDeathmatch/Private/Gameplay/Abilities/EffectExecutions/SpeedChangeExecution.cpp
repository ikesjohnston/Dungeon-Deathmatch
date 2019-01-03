// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedChangeExecution.h"
#include "Abilities/DungeonAttributeSet.h"

struct FSpeedStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MovementSpeed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Finnese);

	FSpeedStatics()
	{
		// Capture the Target's MovementSpeed attribute.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDungeonAttributeSet, MovementSpeed, Target, false);

		// Capture the Target's Finnese attribute.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDungeonAttributeSet, Finnese, Target, false);
	}
};

static const FSpeedStatics& SpeedStatics()
{
	static FSpeedStatics SpeedStatics;
	return SpeedStatics;
}

USpeedChangeExecution::USpeedChangeExecution()
{
	RelevantAttributesToCapture.Add(SpeedStatics().MovementSpeedDef);
	RelevantAttributesToCapture.Add(SpeedStatics().FinneseDef);
}

void USpeedChangeExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float MovementSpeed = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SpeedStatics().MovementSpeedDef, EvaluationParameters, MovementSpeed);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(SpeedStatics().MovementSpeedProperty, EGameplayModOp::Override, MovementSpeed));

	/*float Finnese = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SpeedStatics().FinneseDef, EvaluationParameters, Finnese);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(SpeedStatics().FinneseProperty, EGameplayModOp::Override, Finnese));*/
}
