// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PanicEmitterComponent.generated.h"

/**
 * Component attached to threat actors (e.g., monsters, hazards) that radiates panic influence to nearby panic receivers.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UPanicEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPanicEmitterComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Returns the inner threshold distance below which maximum point-blank panic is applied. */
	float GetMinDistance() const;

	/** Sets the inner point-blank panic distance threshold. */
	void SetMinDistance(float Distance);

	/** Returns the maximum distance beyond which this emitter exerts no panic. */
	float GetOutOfSightDistance() const;

	/** Sets the maximum panic distance threshold. */
	void SetOutOfSightDistance(float Distance);

	/** Returns the base threat rating used in panic accumulation formulas. */
	float GetBaseThreat() const;

	/** Sets the base threat rating. */
	void SetBaseThreat(float InBaseThreat);

	/** Returns the float curve governing distance-based panic falloff. */
	UCurveFloat* GetFalloffCurve() const;

	/** Sets the float curve governing distance-based panic falloff. */
	void SetFalloffCurve(UCurveFloat* InFalloffCurve);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
protected:
	/** Panic Zone Start Distance (in Meters); below this threshold lies the Point Blank Zone where maximum panic is induced. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Panic System", meta=(Units="Meters", ClampMin="0.0"))
	float MinDistance = 1.f;
	
	/** Maximum distance (in Meters) beyond which the player is considered out of sight and safe from this emitter. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Panic System", meta=(Units="Meters", ClampMin="0.0"))
	float OutOfSightDistance = 10.f;
	
	/** Base threat magnitude applied when evaluating panic accumulation rate. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Panic System")
	float BaseThreat = 10.f;

	/** Optional curve modulating threat intensity as distance varies between MinDistance and OutOfSightDistance. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Panic System")
	UCurveFloat* FalloffCurve;
};
