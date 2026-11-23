// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameEndTriggerBox.generated.h"

class UHOERPageWidget;
class AHOERCharacterBase;

/** Delegate broadcast when a valid character enters the game end trigger box. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerGameEnd);

/**
 * Trigger box volume that detects player arrival at the end of the game or level,
 * pushing the victory/game-end UI page and firing completion events.
 */
UCLASS()
class HOER_API AGameEndTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameEndTriggerBox();

	// The event trigger to override
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	/** Broadcast when the game ending sequence is triggered. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTriggerGameEnd OnTriggerGameEnd;
	
#if WITH_EDITOR
	// Called by the editor to validate the asset or level actor
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
protected:
	/** Classes of characters allowed to activate this trigger (typically the player character). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TArray<TSubclassOf<AHOERCharacterBase>> CharactersToDetect;
	
	/** UI page widget class displayed upon reaching the game conclusion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TSubclassOf<UHOERPageWidget> GameEndPageWidget;
	
	/** Identifier name registered in the HUD page stack for the game end screen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	FName GameEndPageName = "GameEnd";
	
	/**
	 * Blueprint event triggered when the game end sequence is activated.
	 * Override in Blueprints to script credits, level transition sequences, or particle fireworks.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events", meta = (DisplayName = "Trigger Game End"))
	void BP_TriggerGameEnd();
	
	void EmitTriggerGameEnd();
	
private:
	/** Cached flag to prevent multiple trigger activations. */
	bool bIsGameEnd = false;
};
