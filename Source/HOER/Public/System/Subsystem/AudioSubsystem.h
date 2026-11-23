// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

class AMusicActor;
class UMetaSoundSource;

/** Delegate fired once an asynchronous MetaSound asset load operation finishes. */
DECLARE_DELEGATE_OneParam(FOnSoundLoaded, UMetaSoundSource*);

/**
 * Game Instance subsystem managing dynamic audio streaming, MetaSound caching, and tension level updates.
 */
UCLASS()
class HOER_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	/**
	 * Asynchronously streams and loads a MetaSound asset, executing the callback upon completion.
	 *
	 * @param Sound Soft reference to the MetaSound asset.
	 * @param OnSoundLoaded Callback delegate executed when the sound asset is loaded.
	 */
	void LoadSoundAsync(TSoftObjectPtr<UMetaSoundSource> Sound, FOnSoundLoaded OnSoundLoaded);

	/**
	 * Registers the active Music Actor to handle dynamic background music playback.
	 *
	 * @param InMusicActor The music actor instance to register.
	 */
	void RegisterMusicActor(AMusicActor* InMusicActor);

	/**
	 * Unregisters the active Music Actor.
	 *
	 * @param InMusicActor The music actor instance to unregister.
	 */
	void UnregisterMusicActor(AMusicActor* InMusicActor);

	/**
	 * Updates the tension level parameter on the active Music Actor.
	 *
	 * @param InTensionLevel Integer tension intensity value.
	 */
	void SetTensionLevel(const int32 InTensionLevel) const;
	
private:
	
	/** Cache of loaded MetaSound sources mapped by asset path to prevent redundant streaming. */
	UPROPERTY()
	TMap<FSoftObjectPath, TObjectPtr<UMetaSoundSource>> CachedSoundSources;
	
	/** Active streamable handles for pending sound loading requests. */
	TMap<FSoftObjectPath, TSharedPtr<FStreamableHandle>> SoundHandles;
	
	/** Queued callback delegates waiting for completion of sound streaming per asset path. */
	TMap<FSoftObjectPath, TArray<FOnSoundLoaded>> WaitingCallbacks;
	
	/** Internal callback triggered when a streamable handle completes loading. */
	void OnLoadCompleted(TSoftObjectPtr<UMetaSoundSource> SoftPtrSound);

	/** Cached pointer to the active level Music Actor. */
	UPROPERTY()
	TObjectPtr<AMusicActor> MusicActor = nullptr;
};
