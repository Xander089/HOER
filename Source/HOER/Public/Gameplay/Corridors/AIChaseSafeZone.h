// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIChaseSafeZone.generated.h"

class UBoxComponent;

/**
 * Volume actor demarcating a designated sanctuary zone where AI enemies cannot chase the player,
 * automatically suspending pursuit and triggering checkpoint auto-save.
 */
UCLASS()
class HOER_API AAIChaseSafeZone : public AActor
{
	GENERATED_BODY()

public:
	AAIChaseSafeZone();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
	/**
	 * Blueprint event triggered when the player character enters this safe zone volume.
	 * Override in Blueprints to script safe zone ambient audio, lighting adjustments, or UI indicators.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnPlayerEnter();
	
protected:

	/** Box collision component detecting when the player enters or exits the safe area. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> TriggerBox = nullptr;
	
private:
	
	void NotifyCorridorManager(const bool bEnteredSafeZone) const;
	void TriggerSaveAsync() const;
};
