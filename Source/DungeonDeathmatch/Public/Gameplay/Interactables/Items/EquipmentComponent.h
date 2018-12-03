// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equippable.h"
#include "EquipmentComponent.generated.h"


DECLARE_DELEGATE_OneParam(FOnItemEquipped, AEquippable*);
DECLARE_DELEGATE_OneParam(FOnItemUnequipped, AEquippable*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONDEATHMATCH_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Equipment")
	TArray<AEquippable*> Equipment;

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
	TArray<AEquippable*> GetEquipment() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AEquippable* GetEquipmentInSlot(EEquipmentSlot Slot) const;

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_EquipItem(AEquippable* Equippable);

	UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	void Multicast_OnEquipItem(AEquippable* Equippable);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_UnequipItem(AEquippable* Equippable);

	UFUNCTION(NetMulticast, Unreliable, Category = "Equipment")
	void Multicast_OnUnequipItem(AEquippable* Equippable);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Equipment")
	void Server_UnequipAll();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool HasItemEquipped(AEquippable* Equippable) const;
};
