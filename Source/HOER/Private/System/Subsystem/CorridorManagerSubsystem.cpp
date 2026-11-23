// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/CorridorManagerSubsystem.h"

#include "AI/Controller/AIDirectorController.h"
#include "AI/Controller/AIEnemyController.h"
#include "Gameplay/Corridors/CorridorSpawnPoint.h"

void UCorridorManagerSubsystem::Register(ACorridorSpawnPoint* InCorridor)
{
	if (IsValid(InCorridor))
	{
		CorridorsSpawnPoints.AddUnique(InCorridor);
	}
}

void UCorridorManagerSubsystem::Unregister(ACorridorSpawnPoint* InCorridor)
{
	CorridorsSpawnPoints.Remove(InCorridor);
}

void UCorridorManagerSubsystem::RegisterAIDirectorController(AAIDirectorController* InAIDirectorController)
{
	AIDirectorController = InAIDirectorController;
}

void UCorridorManagerSubsystem::UnregisterAIDirectorController(const AAIDirectorController* InAIDirectorController)
{
	if (AIDirectorController == InAIDirectorController)
	{
		AIDirectorController = nullptr;
	}
}

void UCorridorManagerSubsystem::RegisterAIEnemyController(AAIEnemyController* InAIEnemyController)
{
	AIEnemyController = InAIEnemyController;
}

void UCorridorManagerSubsystem::UnregisterAIEnemyController(const AAIEnemyController* InAIEnemyController)
{
	if (AIEnemyController == InAIEnemyController)
	{
		AIEnemyController = nullptr;
	}
}

/** 
 * Asks the Corridor Manager Subsystem to select a random spawn point close to the player's current corridor, if specified.
 *
 * @param CurrentPlayerCorridor The current corridor where the player is located
 *
 * @returns The Next ACorridorSpawnPoint* Actor.
 */
ACorridorSpawnPoint* UCorridorManagerSubsystem::SelectNextSpawnPoint(ACorridorSpawnPoint* CurrentPlayerCorridor) const
{
	if (CorridorsSpawnPoints.Num() == 0) { return nullptr; }
	
	if (CurrentPlayerCorridor != nullptr && CorridorsSpawnPoints.Contains(CurrentPlayerCorridor))
	{
		return CurrentPlayerCorridor;
	}
	
	ACorridorSpawnPoint* SelectedCorridor = CorridorsSpawnPoints[FMath::RandRange(0, CorridorsSpawnPoints.Num() - 1)].Get();
	if (!IsValid(SelectedCorridor))	{ return nullptr;}
	return SelectedCorridor;
}

void UCorridorManagerSubsystem::NotifyPlayerReachedSpawnPoint(ACorridorSpawnPoint* CorridorSpawnPoint) const
{
	
	if (!IsValid(CorridorSpawnPoint)) { return; }
	
	if (AIDirectorController.IsValid())
	{
		AIDirectorController->SetPlayerSpawnLocation(CorridorSpawnPoint);
	}
}

void UCorridorManagerSubsystem::NotifyPatrolEnded() const
{
	if (AIDirectorController.IsValid())
	{
		AIDirectorController->NotifyPatrolEnded();
	}
}

void UCorridorManagerSubsystem::NotifyPlayerEnteredSafeZone(const bool bEntered) const
{
	if (AIEnemyController.IsValid())
	{
		AIEnemyController->NotifyPlayerEnteredSafeZone(bEntered);
	}
}

void UCorridorManagerSubsystem::ToggleAIControl(const bool bShouldPause) const
{
	if (AIDirectorController.IsValid())
	{
		AIDirectorController->ToggleControl(bShouldPause);
	}
	if (AIDirectorController.IsValid())
	{
		AIEnemyController->ToggleControl(bShouldPause);
	}
}
