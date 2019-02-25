// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerBrowserLineWidget.generated.h"

class UTextBlock;
class UMainMenuWidget;
class UButton;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UServerBrowserLineWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SessionNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LatencyText;

private:
	UPROPERTY()
	UMainMenuWidget* ParentWidget;

	uint32 ServerIndex;

protected:
	virtual bool Initialize() override;

public:
	void Setup(UMainMenuWidget* Parent, uint32 Index);

	void SetSessionNameText(FText Text);

	void SetPlayerCountText(FText Text);

	void SetLatencyText(FText Text);

protected:
	UFUNCTION()
	void OnSelectButtonPressed();
};
