// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIWaypointsDataAsset.generated.h"

/**
 * Data Asset storing an ordered list of 3D waypoint coordinates defining an enemy patrol path.
 */
UCLASS()
class HOER_API UAIWaypointsDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/** Sequential list of world coordinates defining the AI patrol route. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
	TArray<FVector> Waypoints;
};
