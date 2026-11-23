// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LightManagerSubsystem.generated.h"

class ALightActor;

/** Broadcast when any light actor changes its switched on/off state. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightOn, bool, bOn);

/** Broadcast when an actor enters illumination by a light source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartIlluminate, AActor*, OtherActor);

/** Broadcast when an actor exits illumination by a light source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopIlluminate, AActor*, OtherActor);

/**
 * World subsystem managing all interactive light actors in the level, tracking active lighting history,
 * illumination events, and calculating light fitness metrics.
 */
UCLASS()
class HOER_API ULightManagerSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// ===  Register Pattern  Methods ===
	
	/**
	 * Registers a light actor with the light manager subsystem.
	 *
	 * @param InLight The light actor to register.
	 */
	void Register(ALightActor* InLight);
	
	/**
	 * Unregisters a light actor when it is destroyed or removed.
	 *
	 * @param InLight The light actor to unregister.
	 */
	void Unregister(ALightActor* InLight);

	/**
	 * Pre-allocates internal arrays and lookup maps to hold the specified number of lights.
	 *
	 * @param InLightCount Total expected number of light actors.
	 */
	void Reserve(const int32 InLightCount);
	
	/** Broadcast when any light in the level toggles between on and off. */
	FOnLightOn OnLightStateChanged;

	/** Broadcast when an actor becomes illuminated. */
	FOnStartIlluminate OnStartIlluminate;

	/** Broadcast when an actor is no longer illuminated. */
	FOnStopIlluminate OnStopIlluminate;
	
	/**
	 * Retrieves the last K switched on lights in chronological order from activation history.
	 *
	 * @param K The maximum number of recent lights to retrieve.
	 * @param OutLights Array populated with the matching light actor pointers.
	 */
	void GetLast_K_SwitchedOnLightsFromHistory(const int32 K, TArray<ALightActor*>& OutLights) const;
	
	/**
	 * Returns the total count of registered light actors in the level.
	 *
	 * @return Total number of registered lights.
	 */
	int32 GetLightCount() const;

	/**
	 * Returns the count of lights currently turned on.
	 *
	 * @return Number of switched on lights.
	 */
	int32 GetSwitchedOnLightCount() const;

	/**
	 * Computes a fitness score based on active lights and their spatial adjacency along level corridors.
	 *
	 * @param AdjacentLightsOnCountWeight Weight factor applied to consecutive lit lights.
	 * @return Calculated fitness metric.
	 */
	float CalculateFitness(const float& AdjacentLightsOnCountWeight) const;

protected:
	
	/** Weak references to all registered light actors in the level. */
	UPROPERTY()
	TArray<TWeakObjectPtr<ALightActor>> Lights;

	/** Fast lookup mapping from light actor pointer to its index in the Lights array. */
	UPROPERTY()
	TMap<TWeakObjectPtr<ALightActor>, int32> LightToIndexMap;
	
	/** Chronologically ordered indices of currently switched on lights. */
	UPROPERTY()
	TArray<int32> TurnedOnHistory;

	/** Internal handler bound to LightActor state changes. */
	UFUNCTION()
	void HandleLightStateChanged(ALightActor* InLight);
	
	/** Internal handler triggered when an actor begins receiving light. */
	UFUNCTION()
	void HandleStartIlluminate(AActor* OtherActor) const;

	/** Internal handler triggered when an actor stops receiving light. */
	UFUNCTION()
	void HandleStopIlluminate(AActor* OtherActor) const;
	
private:
	
	/** Updates the chronological history of turned on lights when an actor toggles state. */
	void UpdateLightStateHistory(ALightActor* InLight);
};
