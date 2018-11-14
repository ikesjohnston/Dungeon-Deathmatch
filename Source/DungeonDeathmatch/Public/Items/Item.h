// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Functionable.h"
#include "Item.generated.h"

class ADungeonCharacter;

UCLASS()
class DUNGEONDEATHMATCH_API AItem : public AActor, public IFunctionable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FString ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Value;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* ItemImage;

public:	
	// Sets default values for this actor's properties
	AItem();
	virtual ~AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NativeOnStartPrimaryFunction(ADungeonCharacter* FunctioningCharacter) override;

	virtual void NativeOnFocusStart() override;

	virtual void NativeOnFocusEnd() override;

	UStaticMeshComponent* GetMeshComponent();
	
	virtual void Server_OnPickup(ADungeonCharacter* Character);

	virtual void Server_OnEquip(ADungeonCharacter* Character);
};
