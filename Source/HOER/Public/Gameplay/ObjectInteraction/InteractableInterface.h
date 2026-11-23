// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactive world entities (actors, props, NPCs, mechanisms)
 * to receive player and AI interactions.
 */
class HOER_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to interact with this object.
	 *
	 * @param Interactor The actor initiating the interaction (typically the player character).
	 * @return True if interaction was successful, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool Interact(AActor* Interactor);

	/**
	 * Checks whether this object can currently be interacted with by the given interactor.
	 *
	 * @param Interactor The actor attempting interaction.
	 * @return True if interaction is permitted, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;

	/**
	 * Gets the interaction prompt text to display (e.g. "Press X To Interact").
	 *
	 * @return Localized prompt text for this interactable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt() const;

	/**
	 * Tells wether the player interacted with this object
	 * 
	 * @return True if the player interacted with this object, false otherwise.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool WasInteractedWith() const;


	// Set the interface as interacted with
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetInteractedWith(const bool bInteracted);
	
	virtual bool Interact_Implementation(AActor* Interactor);
	virtual bool CanInteract_Implementation(AActor* Interactor) const;
	virtual FText GetInteractionPrompt_Implementation() const;
	virtual bool WasInteractedWith_Implementation() const;
	virtual void SetInteractedWith_Implementation(bool bInteracted);
};
