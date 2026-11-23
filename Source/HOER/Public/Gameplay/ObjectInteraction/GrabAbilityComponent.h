// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabAbilityComponent.generated.h"

class APushableObject;

/**
 * Delegate broadcast when a pushable object is successfully grabbed.
 *
 * @param GrabbedObject The pushable object that was grabbed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectGrabbedSignature, APushableObject*, GrabbedObject);

/**
 * Delegate broadcast when a grabbed pushable object is released.
 *
 * @param ReleasedObject The pushable object that was released.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectReleasedSignature, APushableObject*, ReleasedObject);

/**
 * Component responsible for managing the character's grab ability and pushable object interaction.
 * Encapsulates the grabbed object reference, state, directional movement input delegation,
 * and lifecycle events, decoupling character locomotion from interactable world objects.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class HOER_API UGrabAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabAbilityComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Checks whether the owner character is currently grabbing an object.
	 *
	 * @return True if currently grabbing a valid pushable object, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Grab Ability")
	bool IsGrabbing() const;

	/**
	 * Returns the currently grabbed pushable object, if any.
	 *
	 * @return The grabbed pushable object actor, or nullptr.
	 */
	UFUNCTION(BlueprintPure, Category = "Grab Ability")
	APushableObject* GetGrabbedObject() const { return GrabbedObject.Get(); }

	/**
	 * Initiates grabbing a pushable object. If another object is already grabbed, it will be released first.
	 *
	 * @param InObjectToGrab The pushable object to grab.
	 * @return True if successfully grabbed, false if the object was invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grab Ability")
	bool Grab(APushableObject* InObjectToGrab);

	/**
	 * Releases the currently grabbed object and halts pushing motion.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grab Ability")
	void ReleaseGrab();

	/**
	 * Forwards 2D directional movement input from the player to the grabbed object.
	 *
	 * @param MovementVector 2D directional input vector from the controller.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grab Ability")
	void ApplyPushInput(const FVector2D& MovementVector);

	/**
	 * Halts pushing motion by sending a zero input vector to the grabbed object.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grab Ability")
	void StopPushInput();

	/**
	 * Attempts to consume movement input if an object is currently grabbed.
	 *
	 * @param MovementVector 2D directional input vector from the controller.
	 * @return True if movement was consumed by the grab ability, false if character should perform normal locomotion.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grab Ability")
	bool TryHandleMovementInput(const FVector2D& MovementVector);

	/** Broadcast when a pushable object is successfully grabbed. */
	UPROPERTY(BlueprintAssignable, Category = "Grab Ability|Events")
	FOnObjectGrabbedSignature OnObjectGrabbed;

	/** Broadcast when a grabbed pushable object is released. */
	UPROPERTY(BlueprintAssignable, Category = "Grab Ability|Events")
	FOnObjectReleasedSignature OnObjectReleased;

protected:
	/** Currently grabbed pushable object actor, if any. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grab Ability")
	TObjectPtr<APushableObject> GrabbedObject = nullptr;
};
