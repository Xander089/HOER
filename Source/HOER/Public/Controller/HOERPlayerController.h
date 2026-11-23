// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/GameTypes.h"
#include "HOERPlayerController.generated.h"

class UInputMappingContext;

/**
 * Player Controller managing input context switching (Gameplay vs UI), gamepad connectivity detection,
 * pause menus, and camera settings.
 */
UCLASS()
class HOER_API AHOERPlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	AHOERPlayerController();
	
	/**
	 * Enhanced Input binding requesting to navigate back / pop the top page in the HUD stack.
	 */
	UFUNCTION()
	void RequestBack() const;
	
	/**
	 * Switches the active Enhanced Input mapping context between Gameplay and UI modes.
	 *
	 * @param InInputType The desired input routing context.
	 */
	void SwapInputMappingContext(GameTypes::EInputType InInputType) const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Checks whether a physical gamepad controller is currently detected and active.
	 *
	 * @return True if a gamepad is connected, false for keyboard/mouse.
	 */
	UFUNCTION(BlueprintCallable, DisplayName = "Is Gamepad Connected")
	FORCEINLINE bool IsGamepadConnected() const { return bIsGamepadConnected; }

	/** Handles pausing logic and pushes the pause menu page. */
	void PauseGame();

	/**
	 * Blueprint event triggered when the game is paused.
	 * Override in Blueprints to script pause visual effects or custom audio filtering.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Pause Game", Category = "Input")
	void BP_PauseGame();

	/**
	 * Sets whether the current map is a main menu level, suppressing pause action in menus.
	 *
	 * @param bInIsMainMenu True if main menu level, false if gameplay.
	 */
	void SetIsMainMenuLevel(bool bInIsMainMenu = false);
	
protected:
	
	UFUNCTION()
	void OnControllerChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
	
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
	/** Default Enhanced Input mapping context applied during active gameplay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Input Mappings")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	/** Enhanced Input mapping context applied when interacting with UI menus and navigation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Input Mappings")
	TObjectPtr<UInputMappingContext> UIMappingContext;

private:
	
	void SetGamepadConnected();
	
	bool bIsGamepadConnected = false;
	bool bIsMainMenuLevel = false;
};
