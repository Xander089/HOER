// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/ObjectInteraction/InteractableInterface.h"
#include "Save/SavableActorInterface.h"
#include "InteractableObject.generated.h"

class APickupableObject;
class USphereComponent;
class UWidgetComponent;

/**
 * Delegate broadcast when an interaction succeeds or fails.
 *
 * @param Interactor The actor performing the interaction (typically the player).
 * @param InteractableObject The interactable actor instance being interacted with.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteraction, AActor*, Interactor, AInteractableObject*, InteractableObject);

/**
 * Delegate broadcast when a required pickupable item is consumed by an interactable object.
 *
 * @param Interactor The actor performing the interaction.
 * @param InteractableObject The interactable actor instance consuming the item.
 * @param ConsumedItemClass The class of the inventory item that was consumed.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemConsumedSignature, AActor*, Interactor, AInteractableObject*, InteractableObject, TSubclassOf<APickupableObject>, ConsumedItemClass);

/**
 * Base actor for interactive world objects (doors, light switches, puzzles, inspectables).
 * Supports item requirements, single-use configurations, and proximity prompt widgets.
 */
UCLASS()
class HOER_API AInteractableObject : public AActor, public IInteractableInterface, public ISavableActorInterface
{
    GENERATED_BODY()

public:
    AInteractableObject();

    /**
     * Attempts to perform an interaction on this object by the specified interactor.
     * Evaluates item requirements and triggers success or failure events accordingly.
     *
     * @param Interactor The actor initiating the interaction.
     * @return True if the interaction succeeded, false if failed or not interactable.
     */
    virtual bool Interact(AActor* Interactor);

    // IInteractableInterface Methods
    virtual bool Interact_Implementation(AActor* Interactor) override;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool WasInteractedWith_Implementation() const override;
    virtual void SetInteractedWith_Implementation(bool bInteracted) override;

    // ISavableActorInterface Methods
	virtual void OnActorSaved() override;
	virtual void OnActorLoaded() override;

    /** Sets whether this interactable object has been interacted with. */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetWasInteractedWith(bool bNewWasInteractedWith);

    /** Fired when all required items are satisfied and interaction succeeds. */
    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnInteraction OnInteractionSuccess;

    /** Fired when the interactor attempts interaction without holding required items or while interaction conditions fail. */
    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnInteraction OnInteractionFailed;

    /** Fired each time a required item is consumed from the player's possession. */
    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnItemConsumedSignature OnItemConsumed;

    /**
     * Checks whether this interactable object currently requires specific items to interact.
     *
     * @return True if there are remaining required items in RequiredItems, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category = "Interaction")
    bool DoesInteractionRequireItems() const;

    /**
     * Checks whether this object is currently receptive to player interaction.
     *
     * @return True if enabled and interactable, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category = "Interaction")
    bool IsInteractable() const { return bIsInteractable; }

    /**
     * Enables or disables interaction with this object at runtime.
     *
     * @param bNewInteractable True to allow interactions, false to disallow.
     */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetInteractable(bool bNewInteractable);

    /**
     * Updates the on-screen prompt text at runtime and refreshes the interaction widget.
     *
     * @param NewPromptText The new localized text message to display.
     */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetPromptText(const FText& NewPromptText);

    /**
     * Returns the proximity trigger sphere component.
     *
     * @return The interaction trigger sphere component.
     */
    FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /**
     * Blueprint event triggered when an interaction successfully completes.
     * Override in Blueprints to script custom gameplay outcomes (e.g. unlock door, play cinematic).
     *
     * @param Interactor The actor that interacted with this object.
     * @param InteractableObject This interactable actor instance.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Interaction Success"))
    void BP_OnInteractionSuccess(AActor* Interactor, AInteractableObject* InteractableObject);

    /**
     * Blueprint event triggered when an interaction attempt fails (e.g. missing key item).
     * Override in Blueprints to script feedback such as locked rattle sounds or HUD prompts.
     *
     * @param Interactor The actor that attempted interaction.
     * @param InteractableObject This interactable actor instance.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Interaction Failed"))
    void BP_OnInteractionFailed(AActor* Interactor, AInteractableObject* InteractableObject);

    /**
     * Blueprint event triggered whenever a required item is consumed during interaction.
     *
     * @param Interactor The actor that provided the consumed item.
     * @param InteractableObject This interactable actor instance.
     * @param ConsumedItemClass The class of the item that was consumed.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Required Item Consumed"))
    void BP_OnItemConsumed(AActor* Interactor, AInteractableObject* InteractableObject, TSubclassOf<APickupableObject> ConsumedItemClass);

    /** Internal collision overlap handler when an actor enters the trigger sphere. */
    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /** Internal collision overlap handler when an actor exits the trigger sphere. */
    UFUNCTION()
    void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /** Localized text displayed by the in-world interaction prompt widget. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FText PromptText = NSLOCTEXT("Interactable", "DefaultPrompt", "Press X To Interact");

    /** If true, this object automatically disables its prompt and trigger after succeeding once. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bSingleUse = false;

    /** Whether this object can currently be interacted with. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bIsInteractable = true;

    /** Whether the player has interacted with this object. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bWasInteractedWith = false;

    /** Classes of pickupable objects required to successfully complete interaction (e.g. keys, puzzle items). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Requirements")
    TSet<TSubclassOf<APickupableObject>> RequiredItems;

    /** Proximity trigger sphere used to detect nearby player characters. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> SphereComponent;

    /** In-world 3D widget component displaying interaction prompts to nearby players. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> InteractionPromptWidget;

    virtual void EmitEvent(AActor* Interactor);
    void EmitFailedEvent(AActor* Interactor);
};
