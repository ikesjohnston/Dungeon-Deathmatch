// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ChestAnimInstance.generated.h"

class AChest;

/**
 * AnimInstance class for all chest actors. Used to drive and replicate opening and closing animations.
 */
UCLASS()
class DUNGEONDEATHMATCH_API UChestAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	AChest* OwningChest;

	/* Flag used to drive the opening and closing of the chest mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	bool bIsOpen;

public:
	UChestAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool GetIsOpen();

	UFUNCTION()
	void SetIsOpen(bool IsOpen);
};
