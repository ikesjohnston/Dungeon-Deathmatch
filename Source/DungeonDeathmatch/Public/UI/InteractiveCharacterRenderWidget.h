// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractiveCharacterRenderWidget.generated.h"

class UImage;
class ACharacterRenderCapture2D;
class UMaterialInstance;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UInteractiveCharacterRenderWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/** The image for displaying the character scene capture */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	UImage* CharacterRenderImage;

	ACharacterRenderCapture2D* RenderCaptureActor;

	/** The material instance to use for the generated render target brush */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UMaterialInstance* RenderCaptureMaterialInstance;

	/** The parameter name of the material instance for the render target texture */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName MaterialInstanceRTParamName;

	/** The size to set for the generated render target brush */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D RenderTargetBrushSize;

	FSlateBrush RenderTargetBrush;

public:
	UInteractiveCharacterRenderWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	void SetRenderCaptureActor(ACharacterRenderCapture2D* NewRenderCaptureActor);

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
