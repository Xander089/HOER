// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveData.generated.h"

/**
 * Serialized snapshot of a character or actor's transform and identity.
 */
USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()

	/** Unique name identifier of the saved actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	FName ActorName;

	/** World position location of the saved actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	FVector Location = FVector::ZeroVector;

	/** World rotation orientation of the saved actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Compares two FActorSaveData instances for equality based on their actor names. */
	bool operator==(const FActorSaveData& Other) const
	{
		return ActorName == Other.ActorName;
	}

	/** Necessary for use in TSet/TMap. */
	friend uint32 GetTypeHash(const FActorSaveData& Data)
	{
		return GetTypeHash(Data.ActorName);
	}
};

/**
 * SaveGame asset object storing persistent game records on disk including light states, player, and enemy data.
 */
UCLASS()
class HOER_API USaveData : public USaveGame
{
	GENERATED_BODY()
	
public:
	
	/** List of light actor names that were switched on at save time. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	TArray<FName> LightActors;

	/** Serialized state data for the player character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FActorSaveData PlayerData;

	/** Serialized state data for the primary AI enemy. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FActorSaveData AIEnemyData;

	// keep a set of all the interactable objects that have been interacted with
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	TSet<FActorSaveData> InteractedInteractables;
	
	// keep a set of all the unlockable objects that are unlocked
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	TSet<FActorSaveData> UnlockedUnlockables;
};
