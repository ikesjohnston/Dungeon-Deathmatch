// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Equippable.h"
#include "EquipmentGlobals.h"
#include "MeshEnums.h"
#include "Armor.generated.h"

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
	TMap<EHumanoidMeshSegment, USkeletalMesh*> ArmorMeshMap;

public:
	AArmor(const FObjectInitializer& ObjectInitializer);

	virtual ~AArmor();

protected:
	virtual void BeginPlay() override;

public:
	EArmorSlot GetArmorSlot();

	TMap<EHumanoidMeshSegment, USkeletalMesh*> GetArmorMeshMap();

protected:
	virtual void ServerOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot) override;
	virtual void MulticastOnEquip_Implementation(AActor* InEquippingActor, EEquipmentSlot EquipmentSlot) override;

	virtual void ServerOnUnequip_Implementation() override;
	virtual void MulticastOnUnequip_Implementation() override;

};
