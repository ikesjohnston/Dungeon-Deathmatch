// Fill out your copyright notice in the Description page of Project Settings.

#include "GMSLobbyWidget.h"

void UGMSLobbyWidget::Setup(int32 MaxPlayerCount)
{
	MaxPlayers = MaxPlayerCount;
	PlayerCountDisplay->SetVisibility(ESlateVisibility::Visible);
	GameCountdownDisplay->SetVisibility(ESlateVisibility::Collapsed);
}

void UGMSLobbyWidget::OnPlayerJoined()
{
	CurrentPlayers++;
}

void UGMSLobbyWidget::OnPlayerLeft()
{
	CurrentPlayers++;
}

void UGMSLobbyWidget::BeginCountdown(float Delay)
{
	GameStartDelay = Delay;
	GameStarting = true;

	PlayerCountDisplay->SetVisibility(ESlateVisibility::Collapsed);
	GameCountdownDisplay->SetVisibility(ESlateVisibility::Visible);
}
