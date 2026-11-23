// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractableDebugSubsystem.generated.h"

class AInteractableObject;

/**
 * World subsystem providing debug visualization and tracking for all active interactable objects.
 */
UCLASS()
class HOER_API UInteractableDebugSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	// Subsystem interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const override { return false; }

	/**
	 * Registers an interactable object for debug visualization.
	 *
	 * @param Interactable The interactable object to track.
	 */
	void RegisterInteractable(AInteractableObject* Interactable);

	/**
	 * Unregisters an interactable object from debug tracking.
	 *
	 * @param Interactable The interactable object to unregister.
	 */
	void UnregisterInteractable(AInteractableObject* Interactable);

private:
	/** Weak references to registered interactables to prevent dangling pointers on actor destruction. */
	TArray<TWeakObjectPtr<AInteractableObject>> RegisteredInteractables;
};
