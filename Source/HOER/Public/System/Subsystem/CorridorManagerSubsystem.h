// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CorridorManagerSubsystem.generated.h"

class AAIDirectorController;
class ACorridorSpawnPoint;
class AAIEnemyController;

/**
 * World subsystem managing corridor spawn points and coordinating AI Director / enemy events.
 */
UCLASS()
class HOER_API UCorridorManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// ===  Register Pattern  Methods ===

	/**
	 * Registers a corridor spawn point with the subsystem.
	 *
	 * @param InCorridor The spawn point actor to register.
	 */
	void Register(ACorridorSpawnPoint* InCorridor);

	/**
	 * Unregisters a corridor spawn point upon destruction or removal.
	 *
	 * @param InCorridor The spawn point actor to unregister.
	 */
	void Unregister(ACorridorSpawnPoint* InCorridor);
	
	/**
	 * Registers the AI Director controller with this manager.
	 *
	 * @param InAIDirectorController The AI Director controller instance.
	 */
	void RegisterAIDirectorController(AAIDirectorController* InAIDirectorController);
	
	/**
	 * Unregisters the AI Director controller.
	 *
	 * @param InAIDirectorController The AI Director controller instance.
	 */
	void UnregisterAIDirectorController(const AAIDirectorController* InAIDirectorController);
	
	/**
	 * Registers the primary AI enemy controller.
	 *
	 * @param InAIEnemyController The AI enemy controller instance.
	 */
	void RegisterAIEnemyController(AAIEnemyController* InAIEnemyController);
	
	/**
	 * Unregisters the AI enemy controller.
	 *
	 * @param InAIEnemyController The AI enemy controller instance.
	 */
	void UnregisterAIEnemyController(const AAIEnemyController* InAIEnemyController);

	// === Spawn Selection ===

	/**
	 * Selects an optimal spawn point in a corridor different from the player's current corridor.
	 *
	 * @param CurrentPlayerCorridor The corridor the player is currently occupying (excluded from selection).
	 * @return Selected spawn point actor, or nullptr if none available.
	 */
	ACorridorSpawnPoint* SelectNextSpawnPoint(ACorridorSpawnPoint* CurrentPlayerCorridor = nullptr) const;
	
	/**
	 * Notifies the AI Director that the player has entered or reached a specific corridor spawn point.
	 *
	 * @param CorridorSpawnPoint The corridor spawn point reached by the player.
	 */
	UFUNCTION()
	void NotifyPlayerReachedSpawnPoint(ACorridorSpawnPoint* CorridorSpawnPoint) const;

	/** Notifies the AI Director that an enemy NPC has completed its assigned patrol path. */
	void NotifyPatrolEnded() const;
	
	/**
	 * Notifies the AI system whether the player has entered or exited a safe zone.
	 *
	 * @param bEntered True if player stepped into a safe zone, false if exited.
	 */
	void NotifyPlayerEnteredSafeZone(const bool bEntered) const;

	/**
	 * Pauses or resumes Behavior Tree execution for active enemy AI controllers.
	 *
	 * @param bShouldPause True to pause AI behavior trees, false to resume.
	 */
	void ToggleAIControl(const bool bShouldPause) const;

protected:

	/** All registered corridor spawn points available in the current level. */
	UPROPERTY()
	TArray<TWeakObjectPtr<ACorridorSpawnPoint>> CorridorsSpawnPoints;
	
	/** Weak reference to the active AI Director controller. */
	TWeakObjectPtr<AAIDirectorController> AIDirectorController = nullptr;
	
	/** Weak reference to the active AI enemy controller. */
	TWeakObjectPtr<AAIEnemyController> AIEnemyController = nullptr;
};
