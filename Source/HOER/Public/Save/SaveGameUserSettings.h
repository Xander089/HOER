// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SaveGameUserSettings.generated.h"

class USoundClass;
class USoundMix;

/**
 * Custom Game User Settings persisting player configuration such as audio volume sliders across game sessions.
 */
UCLASS()
class HOER_API USaveGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	
	USaveGameUserSettings();
	
	virtual void SetToDefaults() override;
	
	/**
	 * Returns the singleton instance of the custom user settings object.
	 *
	 * @return Cast pointer to the active USaveGameUserSettings instance.
	 */
	UFUNCTION(BlueprintPure, Category = "Settings", meta = (DisplayName = "Get Save Game User Settings"))
	static USaveGameUserSettings* GetSaveGameUserSettings();
	
	/**
	 * Updates the music master volume setting.
	 *
	 * @param NewVolume Normalized volume level between 0.0 (silent) and 1.0 (full volume).
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float NewVolume);
 
	/**
	 * Retrieves the current music volume level.
	 *
	 * @return Normalized volume level between 0.0 and 1.0.
	 */
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetMusicVolume() const { return MusicVolume; }
 
	/**
	 * Updates the sound effects (SFX) volume setting.
	 *
	 * @param NewVolume Normalized volume level between 0.0 (silent) and 1.0 (full volume).
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetEffectsVolume(float NewVolume);
 
	/**
	 * Retrieves the current sound effects volume level.
	 *
	 * @return Normalized volume level between 0.0 and 1.0.
	 */
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetEffectsVolume() const { return EffectsVolume; }
	
	/**
	 * Applies current music and SFX volume values to the active sound mix and sound classes in the world.
	 *
	 * @param WorldContextObject Context object used to access the world and sound mix subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio", meta = (WorldContext = "WorldContextObject"))
	void ApplyAudioSettings(const UObject* WorldContextObject) const;

private:
	
	/** Persisted music volume multiplier in configuration file. */
	UPROPERTY(Config)
	float MusicVolume = 0.5f;
	
	/** Persisted sound effects volume multiplier in configuration file. */
	UPROPERTY(Config)
	float EffectsVolume = 0.5f;
};
