// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Item.generated.h"

class ADungeonCharacter;

UCLASS()
class DUNGEONDEATHMATCH_API AItem : public AInteractable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	/** How many pounds does this item weigh? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Weight;

	/** How much gold is this item worth? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Value;

	/** Icon texture for this item to represent it in the UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;

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

	UFUNCTION(BlueprintCallable, Category = "Item")
	FText GetItemName();

	UFUNCTION(BlueprintCallable, Category = "Item")
	float GetWeight();

	UFUNCTION(BlueprintCallable, Category = "Item")
	float GetValue();

	UFUNCTION(BlueprintCallable, Category = "Item")
	UTexture2D* GetIcon();

	virtual void NativeOnInteract(ADungeonCharacter* InteractingCharacter) override;

};
