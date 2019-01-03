// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interactables/Items/Equippable.h"
#include "DungeonCharacter.h"
#include "Armor.generated.h"

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Head			UMETA(DisplayName = "Head"),
	Shoulders		UMETA(DisplayName = "Shoulders"),
	Chest			UMETA(DisplayName = "Chest"),
	Waist			UMETA(DisplayName = "Waist"),
	Legs			UMETA(DisplayName = "Legs"),
	Feet			UMETA(DisplayName = "Feet"),
	Hands			UMETA(DisplayName = "Hands"),

	NUM_ARMOR_SLOTS UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API AArmor : public AEquippable
{
	GENERATED_BODY()

protected:
	/* The visual representation of this item when it isn't equipped, which may be different from when it is equipped*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ArmorMeshComponent;

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

	EArmorSlot GetArmorSlot();

	TMap<EMeshSegment, USkeletalMesh*> GetArmorMeshMap();
};
