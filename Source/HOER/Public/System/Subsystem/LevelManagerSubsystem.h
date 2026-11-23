// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManagerSubsystem.generated.h"

/**
 * Game Instance subsystem managing level transitions and seamless loading screen presentation.
 */
UCLASS(Config = Game)
class HOER_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/** Class of the UI widget displayed during map transitions (configurable in DefaultGame.ini). */
	UPROPERTY(Config, BlueprintReadOnly, Category = "Loading Screen")
	TSoftClassPtr<UUserWidget> LoadingScreenWidgetClass;
	
	/** Minimum display time in seconds for the loading screen to avoid abrupt flickering. */
	UPROPERTY(Config, BlueprintReadOnly, Category = "Loading Screen")
	float MinimumLoadingScreenDisplayTime = 2.0f;

private:

	/** Instantiated loading screen widget instance added to the viewport. */
	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingScreenWidget = nullptr;
	
	/** Callback invoked immediately before level streaming/loading commences. */
	void OnPreLoadMap(const FString& MapName);
	
	/** Callback invoked after the destination world map has completed loading. */
	void OnPostLoadMap(UWorld* LoadedWorld);
	
	/** Attaches engine delegates for map transition events. */
	void SetLoadingScreenBindings();

	/** Cleans up engine delegates for map transition events. */
	void RemoveLoadingScreenBindings();
};
