#pragma once

#include "CoreMinimal.h"
#include "Physics/HOERCollisionChannels.h"
#include "LightInterface.h"
#include "GameFramework/Actor.h"
#include "LightActor.generated.h"

class ALightActor;
class UActorLuminanceComponent;
class USphereComponent;
class ULocalLightComponent;

/**
 * Delegate broadcast when a light actor's on/off state changes.
 *
 * @param InLight The light actor whose state changed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightStateChanged, ALightActor*, InLight);

/**
 * Delegate broadcast when an actor enters this light's illumination volume.
 *
 * @param OtherActor The actor now being illuminated.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartIlluminateOtherActor, AActor*, OtherActor);

/**
 * Delegate broadcast when an actor exits this light's illumination volume.
 *
 * @param OtherActor The actor no longer being illuminated.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopIlluminateOtherActor, AActor*, OtherActor);

/** Global boolean to toggle LightActor's debugging. */
extern bool GShowLightDebug;

/**
 * Cached reflection data for an indirect light bounce pair.
 */
USTRUCT(BlueprintType)
struct FIndirectLightData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	TObjectPtr<AActor> Receiver = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	float CachedReflectance = 0.5f;
};

/**
 * Base light actor computing dynamic direct and indirect luminance on overlapping actors.
 */
UCLASS(Abstract)
class HOER_API ALightActor : public AActor, public ILightInterface
{
	GENERATED_BODY()

protected:
	/** Trigger sphere matching the attenuation radius of the light. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "Based on the Attenuation radius of the light, this is the collider of the sphere"), Category = "Light")
	TObjectPtr<USphereComponent> SphereCollider = nullptr;

	/** Collision trace channel queried for light occlusion and reflection checks. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Collision", meta = (Tooltip = "Trace channel queried for light occlusion and reflection checks."))
	TEnumAsByte<ECollisionChannel> LightTraceChannel = COLLISION_LIGHT;

	/** Local light component providing visual lighting to the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (Tooltip = "The light component of the actor"), Category = "Light")
	TObjectPtr<ULocalLightComponent> LightComponent = nullptr;
	
	/** References to actors currently inside the light's attenuation radius. */
	UPROPERTY(meta = (Tooltip = "Reference to the light that have been found inside the light"))
	TArray<AActor*> ActorsInLight;

	/** References to indirect reflection pairs of light actors with cached surface reflectance. */
	UPROPERTY(meta = (Tooltip = "Reference to the indirect pair of light actors and their cached surface reflectance"))
	TMap<AActor*, FIndirectLightData> IndirectActorsInLight;
	
	/** Whether this light is currently turned on. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Tooltip = "Whether this light is switched on"), Category = "Light")
	bool bOn = false;
	
	bool bInverseFallOff = true;
	float Radius = -1;
	float Intensity = -1;
	float DefaultIntensity = -1;
	float FalloffExponent = -1;
	bool bAllowReflection;
	float IndirectLightingIntensity;

public:
	// Sets default values for this actor's properties
	ALightActor();
	
	/** Broadcast when this light is switched on or off. */
	UPROPERTY(BlueprintAssignable, Category = "Light|Events")
	FOnLightStateChanged OnLightStateChanged;
	
	/** Broadcast when an actor begins receiving illumination from this light. */
	UPROPERTY(BlueprintAssignable, Category = "Light|Events")
	FOnStartIlluminateOtherActor OnStartIlluminateOtherActor;
	
	/** Broadcast when an actor stops receiving illumination from this light. */
	UPROPERTY(BlueprintAssignable, Category = "Light|Events")
	FOnStopIlluminateOtherActor OnStopIlluminateOtherActor;
	
	// ILightInterface Methods
	virtual void ToggleLight_Implementation(bool bOn) override;
	virtual bool IsLightOn_Implementation() const override;

	/**
	 * Sets the light intensity and updates the internal light component.
	 *
	 * @param InIntensity New luminous intensity value.
	 */
	void SetIntensity(float InIntensity);

	/**
	 * Returns the collision trace channel used for light occlusion checks.
	 *
	 * @return The collision channel.
	 */
	UFUNCTION(BlueprintPure, Category = "Light|Collision")
	ECollisionChannel GetLightTraceChannel() const { return LightTraceChannel; }

private:
	// Set the inverse falloff for each subclass of local light component
	template <typename T>
	void SetInverseFalloffVariable(ULocalLightComponent* InLightComponent);
	// Generate the light line trace
	void GenerateLightTrace(AActor* InLightFont, AActor* InLightReceiver, bool bInAllowReflection, float InIntensity);
	// Calculate the light intensity based in a inverse falloff
	float CalculateLightWithInverseFallOff(float InIntensity, float InRadius, float InDistance) const;
	// Calculate the light intensity based in a linear falloff
	float CalculateLightWithLinearFallOff(float InIntensity, float InRadius, float InDistance, float InFalloffExponent) const;
	// Calculate the surface reflectance (albedo luminance) of an actor's material
	float CalculateSurfaceReflectance(const AActor* HitActor) const;
	// Get reflection intensity
	float CalculateReflectionIntensity(const AActor* HitActor) const;
	// Add a new pair of indirect light
	void AddIndirectLight(AActor* InLightSource, AActor* InLightReceiver);
	// Remove a pair of indirect light
	bool RemoveIndirectLight(AActor* InLightSource);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	virtual void Tick(float DeltaTime) override;
	// Calculate the light intensity based from how distant an object is
	virtual float CalculateLightIntensityFromDistance(float InIntensity, float InDistance);
};

template <typename T>
void ALightActor::SetInverseFalloffVariable(ULocalLightComponent* InLightComponent)
{
	(TIsDerivedFrom<T, ULocalLightComponent>::Value, "T must be a local light component");
	if (T* LightClass = Cast<T>(InLightComponent))
	{
		bInverseFallOff = LightClass->bUseInverseSquaredFalloff;
		if (bInverseFallOff)
			FalloffExponent = LightClass->LightFalloffExponent;
	}
}
