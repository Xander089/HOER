// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HOERBlueprintFunctionLibrary.generated.h"

class UHOERBaseWidget;
class UHOERPageWidget;

/**
 * Global Blueprint function library providing gameplay, UI, save/load, input, and subsystem helper nodes.
 */
UCLASS()
class HOER_API UHOERBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Asynchronously saves the current game state to the specified save slot.
	 *
	 * @param InSaveSlot Name of the save game slot file.
	 * @param WorldContextObject Context object used to access the world and save subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	static void AsyncSaveGame(const FString& InSaveSlot, const UObject* WorldContextObject);
	
	/**
	 * Flags that a saved game state should be restored as soon as the next level has finished loading.
	 *
	 * @param WorldContextObject Context object used to access the world and save subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	static void RequestLoadOnNextMap(const UObject* WorldContextObject);
	
	/**
	 * Asynchronously loads game data from the specified save slot into the current session.
	 *
	 * @param InSaveSlot Name of the save game slot file to read from.
	 * @param WorldContextObject Context object used to access the world and save subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	static void AsyncLoadGame(const FString& InSaveSlot, const UObject* WorldContextObject);
	
	/**
	 * Pushes a new UI page widget onto the HUD stack and displays it.
	 *
	 * @param WorldContextObject Context object used to access the player HUD.
	 * @param InPageClass The widget class of the page to instantiate and push.
	 * @param InPageName Unique identifier name for the page.
	 * @param InZOrder Rendering priority / Z-order on the screen viewport.
	 * @param bSwapContext Whether input context should transition to UI mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void AddPage(const UObject* WorldContextObject, TSubclassOf<UHOERPageWidget> InPageClass, const FName& InPageName, int32 InZOrder, bool bSwapContext = true);

	/**
	 * Pops the top UI page from the HUD stack and restores the previous page or gameplay context.
	 *
	 * @param WorldContextObject Context object used to access the player HUD.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void RemovePage(const UObject* WorldContextObject);
	
	/**
	 * Checks whether any physical gamepad controller is currently connected and active.
	 *
	 * @param WorldContextObject Context object used to access the player controller.
	 * @return True if a gamepad is detected, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (WorldContext = "WorldContextObject"))
	static bool IsGamepadConnected(const UObject* WorldContextObject);

	/**
	 * Displays a temporary toast notification message on screen using the designated widget class.
	 *
	 * @param InToastWidgetSubClass The widget class to use for displaying the toast notification.
	 * @param Message Text to display inside the notification.
	 * @param Duration How long the toast remains visible on screen in seconds.
	 * @param WorldContextObject Context object used to access the player HUD.
	 */
	UFUNCTION(BlueprintCallable, Category = "Input", meta = (WorldContext = "WorldContextObject"))
	static void ShowToastNotification(
		TSubclassOf<UHOERBaseWidget> InToastWidgetSubClass, 
		const FText& Message,
		float Duration,
		const UObject* WorldContextObject);

	/**
	 * Pauses or resumes AI controller behavior trees across active enemy NPCs via the corridor manager.
	 *
	 * @param bShouldPause If true, pauses AI behavior tree execution; if false, resumes it.
	 * @param WorldContextObject Context object used to access the corridor manager subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI", meta = (WorldContext = "WorldContextObject"))
	static void ToggleAIControl(const bool bShouldPause, const UObject* WorldContextObject);
	
	/**
	 * Informs the player controller whether the current level is a main menu level to adjust input and camera behaviors.
	 *
	 * @param bIsMainMenu True if current map is a main menu level, false if gameplay level.
	 * @param WorldContextObject Context object used to access the player controller.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI", meta = (WorldContext = "WorldContextObject"))
	static void SetIsMainMenuLevel(const bool bIsMainMenu, const UObject* WorldContextObject);

	/**
	 * Sets the audio tension level parameter in the audio subsystem to drive dynamic music and audio layers.
	 *
	 * @param InTensionLevel Integer tension intensity (e.g., 0 for calm, higher for intense/chase).
	 * @param WorldContextObject Context object used to access the audio subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (WorldContext = "WorldContextObject"))
	static void SetTensionLevel(const int32 InTensionLevel, const UObject* WorldContextObject);
};
