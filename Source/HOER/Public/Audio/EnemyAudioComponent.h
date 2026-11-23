// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAudioComponent.h"
#include "EnemyAudioComponent.generated.h"

/**
 * Audio component attached to AI enemies triggering growls, footsteps, and chase vocals.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UEnemyAudioComponent : public UBaseAudioComponent
{
	GENERATED_BODY()

public:
	UEnemyAudioComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	
	virtual void SubscribeToOwnerInputEvents() override;
	virtual void UnSubscribeFromOwnerInputEvents() override;
	
private:
	
	UFUNCTION()
	void PlayGrunt();
};
