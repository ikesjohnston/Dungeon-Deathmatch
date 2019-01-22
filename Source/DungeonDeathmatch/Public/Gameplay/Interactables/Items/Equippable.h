// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "EquipmentEnums.h"
#include "Equippable.generated.h"

/**
 * Base class for all equippable items in the game.
 */
UCLASS()
class DUNGEONDEATHMATCH_API AEquippable : public AItem
{
	GENERATED_BODY()
	
	friend class UEquipmentComponent;

protected:
	/* A reference to the character currently equipping this item, if any. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	ADungeonCharacter* EquippingCharacter;

public:
	// Sets default values for this actor's properties
	AEquippable(const FObjectInitializer& ObjectInitializer);

	virtual ~AEquippable();

	virtual FText GetInventoryUseTooltipText() override;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnEquip(ADungeonCharacter* NewEquippingCharacter);

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnUnequip();
};
