// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AnimNotify_ReportNoise.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Report Noise"))
class DUNGEONDEATHMATCH_API UAnimNotify_ReportNoise : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float Loudness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float MaxRange = 0.0f;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FName SocketName;

public:
	UAnimNotify_ReportNoise(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
};
