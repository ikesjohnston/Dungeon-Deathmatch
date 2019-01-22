// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonCursorWidget.generated.h"

class AItem;
class UImage;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonCursorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** The image widget for the cursor */
	UPROPERTY(meta = (BindWidget))
	UImage* CursorImage;

	/** The default texture to use for the cursor when not dragging an item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cursor")
	UTexture2D* DefaultCursorTexture;

	/** The default size to use for the cursor when not dragging an item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cursor")
	FVector2D DefaultCursorSize;

	/** The time, in seconds, to wait before retrying binding on failure */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cursor")
	float BindingRetryTime;

private:
	/** Has the cursor been bound to a player controller yet? */
	bool bIsCursorBound;

	/** Timer handle for retrying cursor binding on failure */
	FTimerHandle BindCursorTimerHandle;

public:
	UDungeonCursorWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	void Hide();

	void Show();

protected:
	/** Attempts to get the local player controller and bind the cursor to it */
	void BindToController();

	/** Sets cursor visual properties back to default */
	void ResetCursorProperties();
};
