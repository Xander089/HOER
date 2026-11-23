// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HOERGameMode.generated.h"

/**
 * Base Game Mode for HOER gameplay, managing player spawning and game-over transitions.
 */
UCLASS()
class HOER_API AHOERGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	AHOERGameMode();

	/**
	 * Blueprint event triggered when the game is over (e.g. maximum panic reached or caught by enemy).
	 * Override this event in Blueprints to present custom UI, camera effects, or audio stings.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Trigger Game Over")
	void BP_TriggerGameOver();
	
	/**
	 * Initiates the game-over sequence, disabling input on the given controller and broadcasting the game over event.
	 *
	 * @param CurrentLocalController The player controller to freeze input for.
	 */
	void TriggerGameOver(AController* CurrentLocalController);
	
	/** Applies pending save data once player pawns have been spawned into the world. */
	virtual void StartPlay() override;

private:

	/** Cached flag to ensure game over sequence is executed only once per match. */
	bool bIsGameOver = false;
};
