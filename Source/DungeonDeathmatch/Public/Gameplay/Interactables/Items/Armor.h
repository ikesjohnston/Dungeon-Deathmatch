// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equippable.h"
#include "DungeonCharacter.h"
#include "EquipmentEnums.h"
#include "Armor.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API AArmor : public AEquippable
{
	GENERATED_BODY()

protected:
	/* The slot that this piece of armor should go in when equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	EArmorSlot ArmorSlot;

	/* The mapping of meshes that will be displayed on the character while this armor is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	TMap<EMeshSegment, USkeletalMesh*> ArmorMeshMap;

	// ------------------------ BEGIN EQUIPPABLE OVERRIDES ------------------------
	virtual void OnEquip_Implementation(ADungeonCharacter* NewEquippingCharacter) override;
	// ------------------------ BEGIN EQUIPPABLE OVERRIDES ------------------------

public:
	// Sets default values for this actor's properties
	AArmor(const FObjectInitializer& ObjectInitializer);

	virtual ~AArmor();

protected:
	virtual void BeginPlay() override;

public:
	EArmorSlot GetArmorSlot();

	TMap<EMeshSegment, USkeletalMesh*> GetArmorMeshMap();

};
