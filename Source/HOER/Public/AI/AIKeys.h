#pragma once

#include "UObject/NameTypes.h"
 
/**
 * Shared Blackboard key names utilized by AI Director and Enemy Behavior Trees.
 */
namespace AIBlackboardKeys
{
	/** Blackboard key name for cooldown timer between location assignments. */
	const FName NewAssignLocationCooldownTime = FName(TEXT("NewAssignLocationCooldownTime"));

	/** Blackboard key name (bool) indicating whether the director should assign a new location. */
	const FName ShouldAssignNewLocation = FName(TEXT("ShouldAssignNewLocation"));

	/** Blackboard key name (vector) for the director-assigned target location. */
	const FName AssignedLocation = FName(TEXT("AssignedLocation"));

	/** Blackboard key name (vector) for a randomly selected patrol location. */
	const FName RandomLocation = FName(TEXT("RandomLocation"));

	/** Blackboard key name (float) tracking the current player risk awareness level. */
	const FName RiskAwareness = FName(TEXT("RiskAwareness"));

	/** Blackboard key name (float) for idle wait time upon reaching assigned location. */
	const FName AssignedLocationIdleTime = FName(TEXT("AssignedLocationIdleTime"));

	/** Blackboard key name (float) for idle wait time during random patrol steps. */
	const FName RandomPatrolIdleTime = FName(TEXT("RandomPatrolIdleTime"));

	/** Blackboard key name (object) referencing the tracked player actor. */
	const FName PlayerActor = FName(TEXT("PlayerActor"));

	/** Blackboard key name (bool) indicating whether the player is currently under light illumination. */
	const FName PlayerIlluminated = FName(TEXT("PlayerIlluminated"));

	/** Blackboard key name (object) pointing to the next chosen corridor spawn point actor. */
	const FName NextSpawnPoint = FName(TEXT("NextSpawnPoint"));

	/** Blackboard key name (float) delay duration before enemy teleportation commences. */
	const FName TeleportDelay = FName(TEXT("TeleportDelay"));

	/** Blackboard key name (object) referencing the corridor spawn point currently occupied by the player. */
	const FName CurrentPlayerCorridor = FName(TEXT("CurrentPlayerCorridor"));

	/** Blackboard key name (bool) set when the NPC finishes arriving at its assigned waypoint. */
	const FName NPCReachedWaypoint = FName(TEXT("NPCReachedWaypoint"));

	/** Blackboard key name (bool) indicating whether the player is sheltered inside a safe zone. */
	const FName PlayerInSafeZone = FName(TEXT("PlayerInSafeZone"));
}
