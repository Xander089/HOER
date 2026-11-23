// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAudioComponent.h"
#include "CompanionAudioComponent.generated.h"

/**
 * Audio component attached to the companion plush character to trigger jumps, impacts, and landing squeaks.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UCompanionAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()

public:

	UCompanionAudioComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	
	virtual void SubscribeToOwnerInputEvents() override;
	virtual void UnSubscribeFromOwnerInputEvents() override;
	
private:
	
	UFUNCTION()
	void PlayJumping();
	
	UFUNCTION()
	void PlayOnFirstLanding();
};
