// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Engine/StreamableManager.h>
#include <OnlineSessionInterface.h>

#include "Item.h"
#include "Equippable.h"
#include <GMSMenuInterface.h>
#include <GMSNetworkGlobals.h>
#include <GMSSettingsGlobals.h>
#include "NetworkGlobals.h"

#include "DungeonGameInstance.generated.h"

class UDraggableItemWidget;
class UUserWidget;
class ADungeonGameMode;
class UGMSMenuWidgetBase;
class UGMSLobbyWidget;
class UDungeonSaveGame;

/**
 * 
 */
UCLASS()
class DUNGEONDEATHMATCH_API UDungeonGameInstance : public UGameInstance, public IGMSMenuInterface
{
	GENERATED_BODY()
	
protected:
	FStreamableManager AssetLoader;

protected:
	/** Mapping of colors used in inventory and equipment menu widgets for different quality items */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierColors;

	/** Mapping of colors used in text for different quality items */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TMap<EItemQualityTier, FLinearColor> ItemQualityTierTextColors;

	/** The size of inventory gird slots, which also effects the size of equipment slots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	float InventoryGridSlotSize;

	/* The blueprint UDragAndDropItemWidget subclass to use for inventory and equipment menu interactions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Globals")
	TSubclassOf<UDraggableItemWidget> DragAndDropItemWidgetClass;

	/** Mapping of game mode names to their actor string references, for use by UI when hosting and searching for games */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|General")
	TMap<FString, FString> GameModes;

	/** Mapping of game size names to their enum representations, for use by UI when hosting and searching for games */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|General")
	TMap<FString, EGameSize> GameSizes;

	/** Mapping of game size enums to their respective max player counts, for use by UI when hosting and searching for games */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|General")
	TMap<EGameSize, int32> GameSizePlayerCounts;

	/** Mapping of map names to their object string references, for use by UI when hosting and searching for games */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|General")
	TMap<FString, FString> Maps;

	/* The initial forward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|Loot")
	float LootEjectionForwardForce;

	/* The initial upward force to apply to loot when it is ejected from loot containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Globals\|Loot")
	float LootEjectionUpForce;

	/** Mapping of animation montages to play when sheathing different weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Globals")
	TMap<EWeaponSocketType, UAnimMontage*> SheatheAnimationMontages;

	/** Mapping of animation montages to play when unsheathing different weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Globals")
	TMap<EWeaponSocketType, UAnimMontage*> UnsheatheAnimationMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Game Globals\|General")
	FString SaveGameSlotName = FString("Settings");

	UPROPERTY(EditAnywhere, Category = "Audio Globals")
	USoundMix* SoundMixClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio Globals")
	USoundClass* MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio Globals")
	USoundClass* EffectsSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio Globals")
	USoundClass* UISoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio Globals")
	USoundClass* MusicSoundClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGMSMenuWidgetBase> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGMSMenuWidgetBase> InGameMenuWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGMSLobbyWidget> LobbyWidgetClass;

private:
	UPROPERTY()
	UDungeonSaveGame* GameSettings;

	UPROPERTY()
	UGMSMenuWidgetBase* MainMenuWidget;

	UPROPERTY()
	UGMSMenuWidgetBase* InGameMenuWidget;

	UPROPERTY()
	UGMSLobbyWidget* LobbyWidget;

	IOnlineSessionPtr SessionInterface;

	/** The name of the currently ongoing session */
	FName SessionName;

	/** The currently ongoing session search details, if any */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	/** Desired settings for a game that is in the process of being created and hosted */
	UPROPERTY()
	FGMSHostGameSettings DesiredHostGameSettings;

public:
	UDungeonGameInstance(const FObjectInitializer& ObjectInitializer);

	void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(BlueprintCallable)
	void LoadLobbyDisplay();

	UFUNCTION()
	void HostGame(FGMSHostGameSettings Settings);

	UFUNCTION()
	void JoinGame(uint32 Index);

	UFUNCTION()
	void OnJoinedLobby();

	UFUNCTION()
	void OnLeftLobby();

	UFUNCTION()
	void BeginGameCountdown(float Delay);

	void ExitToMainMenu();

	void ExitToDesktop();

	void RefreshServerList();

	void StartSession();

	int32 GetMaxSessionConnections();

	FString GetSessionMapString();

	FStreamableManager& GetAssetLoader();

	UFUNCTION(BlueprintPure)
	TMap<FString, FString> GetGameModes();

	UFUNCTION(BlueprintPure)
	TMap<FString, FString> GetMaps();

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierColors();

	UFUNCTION(BlueprintPure)
	TMap<EItemQualityTier, FLinearColor> GetItemQualityTierTextColors();

	UFUNCTION(BlueprintPure)
	TSubclassOf<UDraggableItemWidget> GetDragAndDropItemWidgetClass();

	UFUNCTION(BlueprintPure)
	TMap<EWeaponSocketType, UAnimMontage*> GetSheatheAnimationMontages();

	UFUNCTION(BlueprintPure)
	TMap<EWeaponSocketType, UAnimMontage*> GetUnsheatheAnimationMontages();

	UFUNCTION(BlueprintPure)
	float GetInventoryGridSlotSize();

	UFUNCTION(BlueprintPure)
	FVector GetRandomLootEjectionForce();

	void SaveGameSettings();

	UFUNCTION()
	FGMSAudioSettings GetAudioSettings();

	UFUNCTION()
	void SetAudioSettings(FGMSAudioSettings Settings, bool ApplyImmediately = true);

protected:
	void LoadGameSettings();

	void ApplyAudioSettings();

	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool WasSessionCreated);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnDestroySessionComplete(FName SessionName, bool WasSessionDestroyed);

	void OnFindSessionsComplete(bool WasSearchSuccessful);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnJoinedLobby();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnLeftLobby();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastBeginGameCountdown(float Delay);
};
