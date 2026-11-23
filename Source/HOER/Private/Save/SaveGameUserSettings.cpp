// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/SaveGameUserSettings.h"

USaveGameUserSettings::USaveGameUserSettings()
{
	MusicVolume = 1.0f;
	EffectsVolume = 1.0f;
}

USaveGameUserSettings* USaveGameUserSettings::GetSaveGameUserSettings()
{
	return Cast<USaveGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void USaveGameUserSettings::SetMusicVolume(const float NewVolume)
{
	MusicVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
}

void USaveGameUserSettings::SetEffectsVolume(const float NewVolume)
{
	EffectsVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
}

void USaveGameUserSettings::ApplyAudioSettings(const UObject* WorldContextObject) const
{
	if (!IsValid(WorldContextObject)){ return;}
}

void USaveGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	MusicVolume = 1.0f;
	EffectsVolume = 1.0f;
}
