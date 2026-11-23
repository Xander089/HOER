// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "GameFramework/Actor.h"
#include "PushableObject.generated.h"

class UPropAudioComponent;
class UTimelineComponent;
class UCurveFloat;
class ACharacter;

/**
 * Delegate broadcast when a player character begins grabbing and pushing this object.
 *
 * @param Interactor The character pushing the object.
 * @param PushableObject The pushable object actor being pushed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPushStart, AActor*, Interactor, APushableObject*, PushableObject);

/**
 * Delegate broadcast when a player character releases or stops pushing this object.
 *
 * @param Interactor The character releasing the object.
 * @param PushableObject The pushable object actor that stopped being pushed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPushEnd, AActor*, Interactor, APushableObject*, PushableObject);

/**
 * Abstract base actor for heavy objects (crates, blocks) that characters can grab, push, or pull across surfaces.
 */
UCLASS(Abstract, Blueprintable)
class HOER_API APushableObject : public AInteractableObject
{
	GENERATED_BODY()

public:
	APushableObject();
    
	virtual bool Interact(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	
	/** Broadcast when the player grabs this object to initiate pushing. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnPushStart OnPushStart;
	
	/** Broadcast when the player releases this object to stop pushing. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnPushEnd OnPushEnd;
	
	/** Releases the grab and halts object movement. */
	void StopPushing();

	/**
	 * Translates player directional input into aligned pushing motion along the valid movement axis.
	 *
	 * @param MoveInput 2D directional input vector from the player controller.
	 */
	void ApplyPushInput(const FVector2D& MoveInput);

	/**
	 * Checks whether this object is currently grabbed and being pushed.
	 *
	 * @return True if grabbed by an interactor, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsGrabbed() const { return bIsGrabbed; }
    
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/**
	 * Blueprint event triggered when character begins pushing this object.
	 *
	 * @param Interactor The character initiating the push.
	 * @param PushableObject This pushable object instance.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Push Start"))
	void BP_OnPushStart(AActor* Interactor, APushableObject* PushableObject);
	
	/**
	 * Blueprint event triggered when character stops pushing this object.
	 *
	 * @param Interactor The character releasing the push.
	 * @param PushableObject This pushable object instance.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Push End"))
	void BP_OnPushEnd(AActor* Interactor, APushableObject* PushableObject);
    
	/** Movement translation speed while being pushed (in Unreal units per second). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PushSpeed = 100.0f; 
    
	/** Static mesh component representing the physical pushable box or crate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
    
	/** Audio component playing scraping and pushing sounds synchronized with movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPropAudioComponent> PropAudioComponent = nullptr;
	
	void EmitPushStartEvent(AActor* Interactor);
	void EmitPushEndEvent(AActor* Interactor);
	
private:
	FVector PushableDirection;
	
	bool bIsGrabbed = false;
    
	UPROPERTY()
	ACharacter* CurrentInteractor = nullptr;

	bool CanGrab() const;
	bool IsFloorMissing(const FVector& NextLocation) const;
	bool IsObjectOnTop(const FVector& StartLocation) const;
	
	void OnStartPushing();
	void OnStopPushing();
	bool bIsBeingPushed = false;
	
	const float CanGrabTraceRadius = 30.0f;
};
