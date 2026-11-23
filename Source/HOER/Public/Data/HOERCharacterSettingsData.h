// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HOERCharacterSettingsData.generated.h"

class UInputAction;

/**
 * Primary Data Asset holding movement physics, Enhanced Input actions, companion throw tuning, and panic modifiers.
 */
UCLASS()
class HOER_API UHOERCharacterSettingsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** Maximum walking speed in cm/s. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float MaxWalkSpeed = 600.0f;
	
	/** Maximum sprinting speed in cm/s. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float MaxSprintSpeed = 900.0f;
	
	/** Multiplier scaling look input sensitivity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (ClampMin = "0.0"))
	float LookSensitivity = 1.0f;
	
	/** Initial upward vertical impulse speed applied when jumping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float JumpZVelocity = 420.0f;
	
	/** Amount of lateral steering control available while airborne (0.0 to 1.0). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AirControl = 0.2f;
	
	/** Enhanced Input Action for directional locomotion. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UInputAction> MoveAction;
	
	/** Enhanced Input Action for mouse/thumbstick camera look. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UInputAction> LookAction;

	/** Enhanced Input Action for jumping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UInputAction> JumpAction;
	
	/** Enhanced Input Action for sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UInputAction> SprintAction;
	
	/** Enhanced Input Action for charging and throwing the companion character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Companion")
	TObjectPtr<UInputAction> ThrowCompanionCharacterAction;
	
	/** Enhanced Input Action for toggling control between the player and companion plush. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Companion")
	TObjectPtr<UInputAction> SwitchControlAction;
	
	/** Enhanced Input Action for interacting with world objects. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UInputAction> InteractAction;
	
	/** Enhanced Input Action for UI back/cancel navigation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputAction> BackAction;

	/** Enhanced Input Action for pausing the game session. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputAction> PauseAction;
	
	/** Minimum throw velocity impulse when tapping throw without charging. */
	UPROPERTY(EditDefaultsOnly, Category = "Companion|Throw Settings")
	float MinThrowForce = 500.0f;

	/** Maximum throw velocity impulse achieved at full charge. */
	UPROPERTY(EditDefaultsOnly, Category = "Companion|Throw Settings")
	float MaxThrowForce = 2000.0f;

	/** Time in seconds required to charge from MinThrowForce to MaxThrowForce. */
	UPROPERTY(EditDefaultsOnly, Category = "Companion|Throw Settings")
	float MaxChargeTime = 2.0f;
	
	/** Maximum distance the companion can wander from the thrower before snapping back. */
	UPROPERTY(EditDefaultsOnly, Category = "Companion|Throw Settings")
	float MaxCompanionLeashDistance = 1500.0f;

	/** Actor class to spawn into the world when launching the companion. */
	UPROPERTY(EditDefaultsOnly, Category = "Companion")
	TSubclassOf<class ACompanionCharacter> CompanionCharacterClassToSpawn;
	
	/** Panic percentage threshold beyond which player movement begins slowing down. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic System", meta=(Units="Percent", ClampMin="1.0", ClampMax="100.0", UIMin="1.0", UIMax="100.0"))
	float SlowMovementPanicValuePercentageThreshold = 20.0f;
	
	/** Minimum walk speed floor applied when high panic cripples movement. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic System", meta = (ClampMin = "0.0"))
	float MinPanicWalkSpeed = 300.0f;
	
	/** Modified jump velocity applied during high panic states. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic System", meta = (ClampMin = "0.0"))
	float MinPanicJumpZVelocity = 600.0f;
};
