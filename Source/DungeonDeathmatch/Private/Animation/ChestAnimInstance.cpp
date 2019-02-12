// Fill out your copyright notice in the Description page of Project Settings.

#include "ChestAnimInstance.h"

UChestAnimInstance::UChestAnimInstance(const FObjectInitializer& ObjectInitializer)
{

}

void UChestAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningChest = Cast<AChest>(TryGetPawnOwner());
}

void UChestAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

bool UChestAnimInstance::GetIsOpen()
{
	return bIsOpen;
}

 void UChestAnimInstance::SetIsOpen(bool IsOpen)
 {
	 bIsOpen = IsOpen;
 }
