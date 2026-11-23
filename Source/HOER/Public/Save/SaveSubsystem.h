// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

class USaveData;

/**
 * Delegate broadcast when a save game data object is read or written.
 *
 * @param SaveObject Pointer to the active save data object.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGame, USaveData*, SaveObject);

/**
 * Game Instance subsystem coordinating synchronous and asynchronous game saving, loading,
 * and restoring serialized actor states across level transitions.
 */
UCLASS()
class HOER_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	
	/** In-memory cache of current save game record. */
	UPROPERTY()
	TObjectPtr<USaveData> CurrentSaveData;

	/** Name of the active save game slot. */
	FString CurrentSaveSlot;

	/** Default save slot identifier. */
	const FString DefaultSaveSlot = TEXT("DefaultSaveSlot");

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/**
	 * Flags that pending save data should automatically be loaded and applied when the next map finishes opening.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void RequestLoadOnNextMap();

	/**
	 * Initiates asynchronous serialization of world and player data to disk.
	 *
	 * @param InSaveSlot Name of the target save slot file (uses default slot if empty).
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void AsyncSaveGame(const FString& InSaveSlot = TEXT(""));

	/**
	 * Loads save game data from disk and populates internal state.
	 *
	 * @param InSaveSlot Name of the save slot file to load from.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadGame(const FString& InSaveSlot);
	
	/**
	 * Internal completion callback triggered once an asynchronous save operation finishes.
	 *
	 * @param SaveSlot The name of the save slot file.
	 * @param UserIndex Platform user index.
	 * @param bSuccess True if written successfully, false otherwise.
	 */
	UFUNCTION()
	void OnAsyncSaveCompleted(const FString& SaveSlot, int32 UserIndex, bool bSuccess);
	
	/**
	 * Deserializes and applies loaded save game data to matching actors in the specified world.
	 *
	 * @param InWorld The world instance containing actors to restore.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void ApplyToWorld(UWorld* InWorld);

	/** Broadcast when the game state is written to a save object. */
	FOnSaveGame OnWriteGame;

	/** Broadcast when the game state is read from a save object. */
	FOnSaveGame OnReadGame;
	
	void Register(AActor* InActor);
	void Unregister(const AActor* InActor);

private:

	UPROPERTY()
	TMap<FString, AActor*> ActorReferenceMap;

	AActor* FindActorByName(const FString& TargetName);
	
	FDateTime CurrentTime = FDateTime::Now();
	bool bPendingLoad = false;
};
