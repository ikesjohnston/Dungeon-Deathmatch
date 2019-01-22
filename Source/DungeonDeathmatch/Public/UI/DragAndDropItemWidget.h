// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragAndDropItemWidget.generated.h"

class AItem;
class UImage;

/**
 * A widget for visually displaying drag and drop operations for items between inventory and equipment menus
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDragAndDropItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The image widget for the item being dragged */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	UImage* ItemImage;

public:
	UDragAndDropItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	/**
	 * Starts rendering an item image and performing position updates to follow the cursor
	 */
	void StartDragAndDropOperation(AItem* Item);

	/**
	 * Hides the widget and halts position updates
	 */
	void StopDragAndDropOperation();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
