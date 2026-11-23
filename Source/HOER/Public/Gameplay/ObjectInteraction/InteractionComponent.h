// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UHeldItem;

/**
 * Delegate broadcast when an interaction is attempted with a target actor.
 *
 * @param TargetActor The actor that is being interacted with.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionAttemptedSignature, AActor*, TargetActor);

/**
 * Delegate broadcast when an interaction succeeds.
 *
 * @param TargetActor The actor that was interacted with.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccessSignature, AActor*, TargetActor);

/**
 * Delegate broadcast when an interaction fails.
 *
 * @param TargetActor The actor that failed the interaction.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFailedSignature, AActor*, TargetActor);

/**
 * Delegate broadcast when a held item is dropped because no interactable target was in range.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeldItemDroppedSignature);

/**
 * Component responsible for managing interactable object detection, focus, and interaction execution.
 * Decouples interaction logic from specific character or pawn classes.
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class HOER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();
	
	virtual void BeginPlay() override;

	/**
	 * Main interaction entry point (suitable for Enhanced Input binding).
	 * Searches for the best candidate interactable and interacts with it, or drops the held item if none found.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	/**
	 * Performs the interaction with the best candidate interactable.
	 *
	 * @return True if an interaction succeeded or a held item was dropped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool PerformInteraction();

	/**
	 * Finds the best interactable candidate in range of the owner actor.
	 * Evaluates overlapping actors implementing IInteractableInterface.
	 *
	 * @return The closest valid interactable actor, or nullptr if none available.
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* FindBestInteractable() const;

	/**
	 * Checks whether the owner can currently interact with the given target actor.
	 *
	 * @param TargetActor The actor to test for interaction capability.
	 * @return True if valid and receptive to interaction, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool CanInteractWith(AActor* TargetActor) const;

	/**
	 * Returns the currently focused interactable actor, if any.
	 *
	 * @return The focused interactable actor.
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable.Get(); }

	/** Broadcast when an interaction attempt is initiated. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionAttemptedSignature OnInteractionAttempted;

	/** Broadcast when an interaction succeeds. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionSuccessSignature OnInteractionSuccess;

	/** Broadcast when an interaction fails. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionFailedSignature OnInteractionFailed;

	/** Broadcast when a held item is dropped because no interactable was found. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnHeldItemDroppedSignature OnHeldItemDropped;

	
protected:

	/** If true, dropping the held item is triggered when pressing interact while no interactable is in range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	bool bDropHeldItemIfNoInteractable = true;

	/** Cached reference to the owner's held item component, if present. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UHeldItem> HeldItemComponent;

	/** Currently focused interactable actor. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<AActor> CurrentInteractable;
};
