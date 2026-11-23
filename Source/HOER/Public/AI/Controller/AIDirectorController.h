// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/HOERAIController.h"
#include "AIDirectorController.generated.h"

class UAIDirectorConfigDataAsset;
class ALightActor;
class AAIEnemyController;
class ULightManagerSubSystem;
class UNavigationSystemV1;
class ACorridorSpawnPoint;

/**
 * AI Director Controller dynamically managing encounter pacing, player risk calculations,
 * and dispatching enemy NPCs based on light activation patterns and corridor transitions.
 */
UCLASS()
class HOER_API AAIDirectorController : public AHOERAIController
{
	GENERATED_BODY()

public:
	
	/**
	 * Callback responding to light state changes in the level to adjust risk awareness and patrol routes.
	 *
	 * @param bOn True if a light turned on, false if turned off.
	 */
	UFUNCTION()
	void HandleLightStateChanged(const bool bOn);
	
	/**
	 * Callback responding when an actor enters illumination.
	 *
	 * @param OtherActor The actor receiving illumination.
	 */
	UFUNCTION()
	void HandleLightStartIlluminating(AActor* OtherActor);

	/**
	 * Callback responding when an actor ceases being illuminated.
	 *
	 * @param OtherActor The actor leaving illumination.
	 */
	UFUNCTION()
	void HandleLightStopIlluminating(AActor* OtherActor);

	/** Computes and assigns a new patrol or search target location for the enemy AI. */
	UFUNCTION()
	void AssignNewRandomLocation();
	
	/**
	 * Updates director tracking of the player's last known corridor spawn point.
	 *
	 * @param CorridorSpawnPoint The corridor spawn point reached by the player.
	 */
	void SetPlayerSpawnLocation(ACorridorSpawnPoint* CorridorSpawnPoint) const;

	/** Notifies the director that the enemy NPC has completed its patrol route. */
	void NotifyPatrolEnded() const;

protected:
	
	/** Configuration data asset defining cooldowns, weights, and pacing curves for this director. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Data")
	TObjectPtr<UAIDirectorConfigDataAsset> AIConfigDataAsset = nullptr;
	
	/** Weak reference to the controlled AI enemy controller. */
	UPROPERTY()
	TWeakObjectPtr<AAIEnemyController> AIEnemyController = nullptr;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void InitializeBlackboardValues() override;

private:
	
	bool bLatestLightTurnedOnOrOff;
	
	UPROPERTY()
	TWeakObjectPtr<ULightManagerSubSystem> LightManagerSubSystem = nullptr;
	
	/** Subscription to public delegate exposed by light manager */
	void SubscribeToLightManager();
	void GetPlayerLocation(FVector& OutLocation) const;
	void RetrieveLatestLightsTurnedOn(TArray<ALightActor*>& LightActors) const;
	float GetRiskAwareness() const;
	FVector CalculateNewRandomLocation(const FVector& PlayerLocation, const TArray<ALightActor*>& LightActors) const;
	void UpdateBlackboardShouldAssignNewLocation(bool bShouldAssignNewLocation);
	
	// Called at begin play to set AIEnemyController
	void InitializeAIEnemyControllerReference();
	void SetIgnoreDetection(const AActor* OtherActor, const bool bIgnore) const;

public:
	
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
