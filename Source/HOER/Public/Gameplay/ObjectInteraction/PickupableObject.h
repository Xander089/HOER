// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObjectInteraction/InteractableObject.h"
#include "PickupableObject.generated.h"

/**
 * Delegate broadcast when an item is successfully picked up.
 *
 * @param Interactor The actor picking up the item.
 * @param PickupableObject The pickupable object being collected.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickup, AActor*, Interactor, APickupableObject*, PickupableObject);

/**
 * Actor representing an item that can be picked up and held by the player (e.g., keys, tools).
 */
UCLASS()
class HOER_API APickupableObject : public AInteractableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickupableObject();
	
	virtual bool Interact(AActor* Interactor) override;
	
	/** Broadcast when the item is successfully picked up into player custody. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnPickup OnPickupSuccess;

	// Returns the level actor label */
	FString GetSaveLabel() const;

protected:
	
	UPROPERTY(EditInstanceOnly, Category = "Save")
	FString SaveLabel = FString();

	/**
	 * Blueprint event fired when this item is picked up.
	 * Override in Blueprints to trigger custom pickup feedback, audio cues, or animations.
	 *
	 * @param Interactor The actor picking up the item.
	 * @param InteractableObject This pickupable object instance.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Pickup Success"))
	void BP_OnPickupSuccess(AActor* Interactor, APickupableObject* InteractableObject);
	
	/** Visual mesh representing the pickupable item in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	void EmitPickupEvent(AActor* Interactor);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	float DefaultMassOverride = 100.0f;
};
