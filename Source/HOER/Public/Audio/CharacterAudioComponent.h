// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAudioComponent.h"
#include "CharacterAudioComponent.generated.h"

/**
 * Audio component attached to the main character to trigger locomotion footsteps, jumps, throws, and grabs.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UCharacterAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterAudioComponent();
	
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	
	virtual void SubscribeToOwnerInputEvents() override;
	virtual void UnSubscribeFromOwnerInputEvents() override;
	
private:
	
	UFUNCTION()
	void StartPlayingFootsteps();
	
	UFUNCTION()
	void StopPlayingFootsteps();
	
	UFUNCTION()
	void PlayJumping();
	
	UFUNCTION()
	void PlayThrowingSound();
	
	UFUNCTION()
	void PlayGrabbingSound();
};
