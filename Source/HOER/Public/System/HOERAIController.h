// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HOERAIController.generated.h"

class UBehaviorTreeComponent;
struct FAIStimulus;
class UAIPerceptionComponent;

/**
 * Base AI Controller class providing navigation validation, behavior tree lifecycle control, and blackboard management.
 */
UCLASS()
class HOER_API AHOERAIController : public AAIController
{
	GENERATED_BODY()

public: 
	
	AHOERAIController();

	/**
	 * Projects a candidate world location onto the navigation mesh to find a valid navigable point.
	 *
	 * @param InCandidateLocation Input location to validate; updated in-place with the projected navigable location.
	 * @param BoxExtent Search query box half-extent around the candidate location.
	 * @return True if a valid navigable point was found, false otherwise.
	 */
	virtual bool ValidateLocation(FVector& InCandidateLocation, const float BoxExtent = 100.0f) const;

	/**
	 * Pauses or resumes the Behavior Tree execution (e.g. when the game is paused or during cutscenes).
	 *
	 * @param bShouldPause True to pause the behavior tree logic, false to resume it.
	 */
	virtual void ToggleControl(const bool bShouldPause) const;

	/** Stops the active Behavior Tree execution completely. */
	void StopControl() const;
	
protected:
	
	virtual void OnPossess(APawn* InPawn) override;
	
	/** Initializes default key-value pairs on the Blackboard component. */
	virtual void InitializeBlackboardValues();
	
	/** Component running the Behavior Tree logic for this controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
 
	/** Blackboard component holding runtime memory and state keys for the Behavior Tree. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	/** Reference to the Behavior Tree asset asset assigned to drive this AI agent. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset = nullptr;
};
