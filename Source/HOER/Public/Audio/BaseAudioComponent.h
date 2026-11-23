// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "BaseAudioComponent.generated.h"

class UAudioSettingsData;
class UMetaSoundSource;
struct FAAudioSettingsData;

/**
 * Base audio component providing asynchronous MetaSound asset streaming, event triggering,
 * and ADSR-envelope fade out control.
 */
UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UBaseAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseAudioComponent();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Soft reference to the MetaSound asset streamed and instantiated for this audio component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<UMetaSoundSource> AudioAsset;

	/** Configuration data asset defining sound concurrency, volume thresholds, and attenuation profiles. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAudioSettingsData> AudioConfig;
	
	virtual void LoadMetaSoundAsync();
	virtual void OnMetaSoundLoaded();

	/**
	 * Stops sound playback smoothly triggering the MetaSound graph's ADSR release fade-out.
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	virtual void StopSoundEventFadeOut();

protected:
	
	virtual void SubscribeToOwnerInputEvents();
	virtual void UnSubscribeFromOwnerInputEvents();
	virtual bool CheckAudioComponentStateValidity();
	
public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
};
