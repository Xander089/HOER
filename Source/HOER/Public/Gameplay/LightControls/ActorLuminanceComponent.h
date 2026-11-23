// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SharedPointer.h"
#include "ActorLuminanceComponent.generated.h"

class ALightActor;

/**
 * Delegate broadcast when the total luminance falling on this actor changes.
 *
 * @param TotalLuminance The aggregate luminance value after recalculation.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightChange, float, TotalLuminance);

/**
 * Component attached to an actor to compute, track, and report the amount of illumination received from active lights.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UActorLuminanceComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Aggregate luminance quantity currently received by this actor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Tooltip = "this variable stores the quantity of light that affect this actor"), Category = "Light")
	float LuminanceValue;
	
	UPROPERTY()
	TMap<AActor*, float> LuminanceValueForLight = TMap<AActor*, float>();

public:
	/** Broadcast whenever the total received luminance value changes. */
	UPROPERTY(BlueprintAssignable, Category = "Light|Events")
	FOnLightChange OnLightChange;
	// Sets default values for this component's properties
	UActorLuminanceComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:
	/**
	 * Returns the current total luminance affecting this actor.
	 *
	 * @return Current aggregate luminance value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Light")
	float GetLuminanceValue() const { return LuminanceValue; }
	
	/**
	 * Registers or updates luminance contribution from a specific light source.
	 *
	 * @param lightFont The light actor contributing illumination.
	 * @param value The luminance intensity contribution to add.
	 * @return The new total aggregate luminance value.
	 */
	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Add a value of luminance based on a light actor, returns the total"), Category = "Light")
	const float AddLuminance(AActor* lightFont, float value);

	/**
	 * Removes luminance contribution from a specific light source when illumination ceases.
	 *
	 * @param lightFont The light actor whose illumination is ending.
	 */
	UFUNCTION(BlueprintCallable, Category = "Light")
	void RemoveLuminance(AActor* lightFont);

	const float SetTotalLuminance();
	const float AddToTotalLuminance(float value);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
