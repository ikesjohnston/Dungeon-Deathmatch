// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class AItem;

DECLARE_DELEGATE_OneParam(FOnItemEquipped, AItem*);
DECLARE_DELEGATE_OneParam(FOnItemUnequipped, AItem*);

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	MainHand					UMETA(DisplayName = "Main Hand"),
	OffHand						UMETA(DisplayName = "Off Hand"),
	Head						UMETA(DisplayName = "Head"),
	Body						UMETA(DisplayName = "Body"),

	NUM_EQUIPMENT_SLOTS
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	TArray<AItem*> Equipment;

private:

	FOnItemEquipped OnItemEquipped;

	FOnItemUnequipped OnItemUnequipped;

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AItem*> GetEquipment() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AItem* GetEquipmentInSlot(EEquipmentSlot Slot) const;

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_EquipItem(AItem* Item);

	UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	void Multicast_OnEquipItem(AItem* Item);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_UnequipItem(AItem* Item);

	UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	void Multicast_OnUnequipItem(AItem* Item);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_UnequipAll();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool HasItemEquipped(AItem* Item) const;
};
