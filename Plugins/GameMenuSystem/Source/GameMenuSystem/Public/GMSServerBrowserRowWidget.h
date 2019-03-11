// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GMSServerBrowserRowWidget.generated.h"

class UTextBlock;
class UGMSMainMenuWidget;
class UButton;

/**
 * Widget class for displaying properties of a single server in a server browser
 */
UCLASS()
class GAMEMENUSYSTEM_API UGMSServerBrowserRowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	bool Selected;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SessionNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LatencyText;

private:
	UPROPERTY()
	UGMSMainMenuWidget* ParentWidget;

	/** The index of the server in relation to the server search results it was found in */
	uint32 ServerIndex;

protected:
	virtual bool Initialize() override;

public:
	/** Performs initial setup to associate this widget with the correct server */
	void Setup(UGMSMainMenuWidget* Parent, uint32 Index);

	void SetSessionNameText(FText Text);

	void SetHostNameText(FText Text);

	void SetPlayerCountText(FText Text);

	void SetLatencyText(FText Text);

protected:
	UFUNCTION()
	void OnSelectButtonPressed();
};
