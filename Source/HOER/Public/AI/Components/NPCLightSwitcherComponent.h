// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCLightSwitcherComponent.generated.h"

class ALightActor;

/**
 * Component that automatically turns off lights near the enemy NPC as environmental feedback of its presence,
 * restoring light states after a configured delay once the NPC leaves.
 */
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class HOER_API UNPCLightSwitcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UNPCLightSwitcherComponent();

	/**
	 * Evaluates proximity to a light actor and switches it off if within range.
	 *
	 * @param Light The candidate light actor.
	 * @return True if light was affected, false otherwise.
	 */
	bool NotifyNearby(AActor *Light);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/** Interval in seconds between periodic spatial queries for nearby lights. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lights", meta = (ClampMin = 0.05, Units="Seconds"))
	float CheckInterval = 0.25f;

	/** Delay in seconds after the NPC vacates the vicinity before restoring the light back on (<= 0 never restores). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lights", meta = (ClampMin = 1.0f, Units="Seconds"))
	float RestoreDelay = 10.0f;

	/** Radius around the NPC within which lights will be toggled off. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lights", meta = (ClampMin = 0.0f, Units="cm"))
	float SwitchOffRadius = 200.0f;

private:

	void RestoreNearbyLightsStatus();

	/** Map of lights switched off by this component alongside their shutdown timestamps. */
	TMap<TWeakObjectPtr<AActor>, double> SwitchedOffLights;
};
