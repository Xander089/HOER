// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Audio/BaseAudioComponent.h"
#include "PropAudioComponent.generated.h"

class UMetaSoundSource;

/**
 * Audio component designed for world props, puzzles, and interactive objects (pushable blocks, doors).
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UPropAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()

public:
	
	UPropAudioComponent();

	virtual void BeginPlay() override;
	virtual void Play(float StartTime) override;
	
	virtual void LoadMetaSoundAsync() override;
	void OnSoundLoaded(UMetaSoundSource* InSound);

protected:
	
	/** The specific trigger input pin name defined on the MetaSound graph to fire. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio Events", meta=(GetOptions="GetEventNameOptions"))
	FName EventInputName = NAME_None;
	
	/** Whether sound playback should automatically begin as soon as asynchronous asset loading finishes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio Events")
	bool bPlayAfterLoading = false;
	
	UFUNCTION()
	TArray<FName> GetEventNameOptions() const;
	
private:
	
	bool bAudioAssetLoaded = false;	
};
