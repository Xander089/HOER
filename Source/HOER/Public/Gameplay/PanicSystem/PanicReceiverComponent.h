// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PanicReceiverComponent.generated.h"

class UPanicProfileDataAsset;

/**
 * Delegate broadcast when the character transitions into a different panic tier/state.
 *
 * @param NewState Struct containing threshold and profile parameters for the new panic state.
 * @param CurrentPanicValue The exact panic value at the moment of state transition.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPanicStateChanged, const FPanicStateDefinition&, NewState, float,
                                             CurrentPanicValue);

/**
 * Delegate broadcast whenever the numeric panic value is updated.
 *
 * @param CurrentPanicValue The newly calculated panic magnitude.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPanicValueChanged, float, CurrentPanicValue);

/**
 * Component attached to actors (such as the player character) that can experience panic,
 * tracking line of sight to panic emitters and managing panic accumulation, decay, and state transitions.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UPanicReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UPanicReceiverComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/**
	 * Returns the current panic level accumulated by this receiver.
	 *
	 * @return Current panic scalar value.
	 */
	UFUNCTION(BlueprintGetter)
	float GetCurrentPanicLevel() const;
	
	/**
	 * Sets the current panic level directly (clamped between minimum and maximum allowable panic).
	 *
	 * @param InCurrentPanicLevel The new panic value to set.
	 */
	UFUNCTION(BlueprintSetter)
	void SetCurrentPanicLevel(float InCurrentPanicLevel);
	
	/**
	 * Checks whether this receiver has an unblocked line of sight and cone of vision toward the target actor.
	 *
	 * @param TargetActor The emitter actor being evaluated.
	 * @return True if visible, false if occluded or outside the field of view.
	 */
	bool CanSeeActor(const AActor* TargetActor) const;

	/**
	 * Computes the starting world location for line-of-sight raycasts (e.g. from the eye socket).
	 *
	 * @return World space coordinate representing the eyes of the receiver.
	 */
	FVector GetSightLocation() const;

	/** Returns the rate per second at which panic dissipates when not exposed to emitters. */
	float GetDecayRate() const;

	/** Broadcast when the receiver transitions across defined panic thresholds (e.g. Calm to Panicked). */
	UPROPERTY(BlueprintAssignable, Category = "Panic System|Events")
	FOnPanicStateChanged OnPanicStateChanged;
	
	/** Broadcast continuously as panic level increases or decays. */
	UPROPERTY(BlueprintAssignable, Category = "Panic System|Events")
	FOnPanicValueChanged OnPanicValueChanged;
	
	/** Returns minimum panic clamp bound from the panic profile. */
	float GetMinPanicLevel() const;

	/** Returns maximum panic clamp bound from the panic profile. */
	float GetMaxPanicLevel() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
protected:
	/** If true, the receiver only panics if the emitter is in front within the field of view cone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Panic System|Sight")
	bool bUseConeOfVision = true;

	/** The total field of view cone in degrees (e.g., 180 means 90 degrees left and 90 degrees right of facing). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Panic System|Sight", meta=(EditCondition="bUseConeOfVision", ClampMin="1.0", ClampMax="360.0"))
	float FieldOfViewDegrees = 180.f;
	
	/** Speed at which panic decreases per second while in safety or darkness. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Panic System|Sight", meta=(ClampMin="0.0"))
	float DecayRate = 10.f;

	/** Mesh socket name to trace line of sight from (e.g., 'head'). If None, uses actor origin. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Panic System|Sight")
	FName EyeSocketName = "head";

	/** Collision trace channel queried for line-of-sight occlusion checks. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Panic System|Sight")
	TEnumAsByte<ECollisionChannel> SightCollisionChannel = ECC_Visibility;
	
	/** Data asset containing threshold definitions, speed modifiers, and post-process curves for panic stages. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic System")
	UPanicProfileDataAsset* PanicProfile;
	
private:
	/** Current panic level accumulated by the receiver. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = GetCurrentPanicLevel, BlueprintSetter = SetCurrentPanicLevel, Category = "Panic System", meta=(AllowPrivateAccess="true"))
	float CurrentPanicLevel = 0.f;
	
	UPROPERTY()
	FName CurrentStateName;
};
