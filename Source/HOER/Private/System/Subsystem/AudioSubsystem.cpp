// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/AudioSubsystem.h"
#include "MetasoundSource.h"
#include "Audio/MusicActor.h"
#include "Engine/AssetManager.h"

void UAudioSubsystem::LoadSoundAsync(TSoftObjectPtr<UMetaSoundSource> Sound, FOnSoundLoaded OnSoundLoaded)
{
	// Check incoming sound 
	if (Sound.IsNull()){return;} // property was not set in the incoming actor 
	
	const FSoftObjectPath& AssetSoftObjPathKey = Sound.ToSoftObjectPath();
	
	// 1. If already loaded --> broadcast immediately
	if (const TObjectPtr<UMetaSoundSource>* FoundSound =  CachedSoundSources.Find(AssetSoftObjPathKey))
	{
		if (!IsValid(*FoundSound)){ return; }
		
		OnSoundLoaded.ExecuteIfBound(*FoundSound);
		return;
	}
	
	// 2. Store the pending callback
	WaitingCallbacks.FindOrAdd(AssetSoftObjPathKey).Emplace(OnSoundLoaded);
	
	// 3. Check: early return if loading process still active
	if (const TSharedPtr<FStreamableHandle>* FoundSoundHandle = SoundHandles.Find(AssetSoftObjPathKey))
	{
		if (FoundSoundHandle->IsValid() && (*FoundSoundHandle)->IsActive()){ return; }
	}
	
	// 4. Async Load the Sound Asset
	const TSharedPtr<FStreamableHandle> SoundHandle =  UAssetManager::GetStreamableManager()
	.RequestAsyncLoad(
		AssetSoftObjPathKey, 
		FStreamableDelegate::CreateUObject(this, &UAudioSubsystem::OnLoadCompleted, Sound)
	);
	
	if (!SoundHandles.Contains(AssetSoftObjPathKey))
	{
		SoundHandles.Add(AssetSoftObjPathKey, SoundHandle);
	}
}

void UAudioSubsystem::RegisterMusicActor(AMusicActor* InMusicActor)
{
	if (IsValid(InMusicActor))
	{
		MusicActor = InMusicActor;
	}
}

void UAudioSubsystem::UnregisterMusicActor(AMusicActor* InMusicActor)
{
	if (IsValid(MusicActor) && MusicActor == InMusicActor)
	{
		MusicActor = nullptr;
	}
}

void UAudioSubsystem::SetTensionLevel(const int32 InTensionLevel) const
{
	if (IsValid(MusicActor))
	{
		MusicActor->SetTensionLevel(InTensionLevel);
	}
}

void UAudioSubsystem::OnLoadCompleted(const TSoftObjectPtr<UMetaSoundSource> SoftPtrSound)
{
	if (SoftPtrSound.IsNull()){return;} // property was not set in the incoming actor 
	if (!SoftPtrSound.IsValid()){ return; }
	
	const FSoftObjectPath& AssetSoftObjPathKey = SoftPtrSound.ToSoftObjectPath();

	// Cache the loaded source in the map
	if (!CachedSoundSources.Contains(AssetSoftObjPathKey))
	{
		CachedSoundSources.Add(AssetSoftObjPathKey, SoftPtrSound.Get());
	}
	
	// fire the pending callbacks
	if (TArray<FOnSoundLoaded>* CallbacksPtr = WaitingCallbacks.Find(AssetSoftObjPathKey))
	{
		if (!CallbacksPtr){ return; }
		
		// Broadcast
		for (const FOnSoundLoaded& Callback : *CallbacksPtr)
		{
			Callback.ExecuteIfBound(SoftPtrSound.Get());
		}
		
		// Reset
		CallbacksPtr->Empty();
	}
	
}
