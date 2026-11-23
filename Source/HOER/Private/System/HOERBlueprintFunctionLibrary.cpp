// Fill out your copyright notice in the Description page of Project Settings.

#include "System/HOERBlueprintFunctionLibrary.h"

#include "Controller/HOERPlayerController.h"
#include "Save/SaveSubsystem.h"
#include "System/Subsystem/AudioSubsystem.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"
#include "UI/Actors/HOERHUD.h"

class UCorridorManagerSubsystem;

void UHOERBlueprintFunctionLibrary::AsyncSaveGame(const FString& InSaveSlot, const UObject* WorldContextObject)
{
	// Resolve current world from context object
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Route asynchronous save request through SaveSubsystem
	if (USaveSubsystem* SaveSubsystem = InWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>())
	{
		SaveSubsystem->AsyncSaveGame(InSaveSlot);
	}
}

void UHOERBlueprintFunctionLibrary::RequestLoadOnNextMap(const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Mark next level transition to restore saved game data
	if (USaveSubsystem* SaveSubsystem = InWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>())
	{
		SaveSubsystem->RequestLoadOnNextMap();
	}
}

void UHOERBlueprintFunctionLibrary::AsyncLoadGame(const FString& InSaveSlot, const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Request save game loading into current active world
	if (USaveSubsystem* SaveSubsystem = InWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>())
	{
		SaveSubsystem->LoadGame(InSaveSlot);
	}
}

void UHOERBlueprintFunctionLibrary::AddPage(
	const UObject* WorldContextObject, 
	TSubclassOf<UHOERPageWidget> InPageClass,
	const FName& InPageName, 
	int32 InZOrder,
	bool bSwapContext
	)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Access player controller HUD and push new page to the UI stack
	if (AHOERHUD* HOERHUD = Cast<AHOERHUD>(InWorld->GetFirstPlayerController()->GetHUD()); IsValid(HOERHUD))
	{
		HOERHUD->AddPage(InPageClass, InPageName, InZOrder, bSwapContext);
	}
}

void UHOERBlueprintFunctionLibrary::RemovePage(const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Access player controller HUD and pop current top page
	if (AHOERHUD* HOERHUD = Cast<AHOERHUD>(InWorld->GetFirstPlayerController()->GetHUD()); IsValid(HOERHUD))
	{
		HOERHUD->RemovePage();
	}
}

bool UHOERBlueprintFunctionLibrary::IsGamepadConnected(const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return false;
	}
	
	// Query player controller's cached hardware connection status
	if (const AHOERPlayerController* CurrentController = Cast<AHOERPlayerController>(InWorld->GetFirstPlayerController()); IsValid(CurrentController))
	{
		return CurrentController->IsGamepadConnected();
	}
	
	return false;
}

void UHOERBlueprintFunctionLibrary::ShowToastNotification(const TSubclassOf<UHOERBaseWidget> InToastWidgetSubClass,
	const FText& Message, const float Duration, const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Forward toast notification display to HUD
	if (AHOERHUD* HOERHUD = Cast<AHOERHUD>(InWorld->GetFirstPlayerController()->GetHUD()); IsValid(HOERHUD))
	{
		HOERHUD->ShowToastNotification(InToastWidgetSubClass, Message, Duration);
	}
}

void UHOERBlueprintFunctionLibrary::ToggleAIControl(const bool bShouldPause, const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Delegate AI Behavior Tree pausing to CorridorManagerSubsystem
	if (const UCorridorManagerSubsystem* CorridorManager = InWorld->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
	{
		CorridorManager->ToggleAIControl(bShouldPause);
	}
}

void UHOERBlueprintFunctionLibrary::SetIsMainMenuLevel(const bool bIsMainMenu, const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	// Configure player controller main menu state flag
	if (AHOERPlayerController* CurrentController = Cast<AHOERPlayerController>(InWorld->GetFirstPlayerController()); IsValid(CurrentController))
	{
		CurrentController->SetIsMainMenuLevel(bIsMainMenu);
	}
}

void UHOERBlueprintFunctionLibrary::SetTensionLevel(const int32 InTensionLevel, const UObject* WorldContextObject)
{
	const UWorld* InWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(InWorld))
	{
		return;
	}
	
	const UGameInstance* GI = InWorld->GetGameInstance();
	if (!IsValid(GI))
	{
		return;
	}

	// Update music tension layer in the AudioSubsystem
	const UAudioSubsystem* AudioSubsystem = GI->GetSubsystem<UAudioSubsystem>();
	if (!IsValid(AudioSubsystem))
	{
		return;
	}
	
	AudioSubsystem->SetTensionLevel(InTensionLevel);
}
