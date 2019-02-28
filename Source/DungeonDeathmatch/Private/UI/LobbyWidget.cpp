// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidget.h"

void ULobbyWidget::Setup(int32 MaxPlayerCount)
{
	MaxPlayers = MaxPlayerCount;
	PlayerCountDisplay->SetVisibility(ESlateVisibility::Visible);
	GameCountdownDisplay->SetVisibility(ESlateVisibility::Collapsed);
}

void ULobbyWidget::OnPlayerJoined()
{
	CurrentPlayers++;
}

void ULobbyWidget::OnPlayerLeft()
{
	CurrentPlayers++;
}

void ULobbyWidget::BeginCountdown(float Delay)
{
	GameStartDelay = Delay;
	GameStarting = true;

	PlayerCountDisplay->SetVisibility(ESlateVisibility::Collapsed);
	GameCountdownDisplay->SetVisibility(ESlateVisibility::Visible);
}
