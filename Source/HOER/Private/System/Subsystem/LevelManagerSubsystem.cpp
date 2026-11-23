// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/LevelManagerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "MoviePlayer.h"


void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetLoadingScreenBindings();
}


void ULevelManagerSubsystem::Deinitialize()
{
	RemoveLoadingScreenBindings();
	Super::Deinitialize();
}

void ULevelManagerSubsystem::OnPreLoadMap(const FString& MapName)
{
	// 1. Get a GameInstance reference
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)){ return; }
	
	// 2. Create the loading screen widget
	if (UClass* WidgetClass = LoadingScreenWidgetClass.LoadSynchronous(); IsValid(WidgetClass))
	{
		LoadingScreenWidget = CreateWidget<UUserWidget>(GameInstance, WidgetClass);
	}
	
	// 3. Define the loading screen attributes
	FLoadingScreenAttributes Attributes;
	Attributes.bAutoCompleteWhenLoadingCompletes = true;
	Attributes.MinimumLoadingScreenDisplayTime = MinimumLoadingScreenDisplayTime;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bWaitForManualStop = false;
	Attributes.WidgetLoadingScreen = IsValid(LoadingScreenWidget) 
		? LoadingScreenWidget->TakeWidget()
		: FLoadingScreenAttributes::NewTestLoadingScreenWidget(); // Epic Games provides a default loading screen widget. Used as a fallback.
	
	// 4. Set up the loading screen with the defined attributes
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
}

void ULevelManagerSubsystem::OnPostLoadMap(UWorld* LoadedWorld)
{
	LoadingScreenWidget = nullptr;
}

void ULevelManagerSubsystem::SetLoadingScreenBindings()
{
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ULevelManagerSubsystem::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelManagerSubsystem::OnPostLoadMap);
}

void ULevelManagerSubsystem::RemoveLoadingScreenBindings()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	LoadingScreenWidget = nullptr;
}