// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorridorSpawnPoint.generated.h"

class UBoxComponent;

/**
 * Waypoint and trigger volume placed in a corridor where enemy NPCs can spawn or teleport.
 * Walking through its trigger informs the AI corridor manager of player movement.
 */
UCLASS()
class HOER_API ACorridorSpawnPoint : public AActor
{
	GENERATED_BODY()

public:

	ACorridorSpawnPoint();

	/**
	 * Returns the target patrol destination waypoint vector associated with this corridor.
	 *
	 * @return Vector position of the patrol waypoint.
	 */
	FVector& GetWaypointVector();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Trigger box detecting when the player enters this corridor segment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> TriggerBox = nullptr;

	/** World-relative location of the waypoint destination inside the corridor for AI patrol. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
	FVector WaypointLocation = FVector::ZeroVector;
};
