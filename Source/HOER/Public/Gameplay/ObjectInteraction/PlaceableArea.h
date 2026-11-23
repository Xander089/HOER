// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "UnlockableInterface.h"
#include "PlaceableArea.generated.h"

class APickupableObject;

/**
 * Delegate broadcast when a required item is placed into this area/socket.
 *
 * @param Interactor The actor that placed the item.
 * @param PlaceableArea The placeable area actor receiving the item.
 * @param PickupableObject The class of the placed item.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemPlaced, AActor*, Interactor, APlaceableArea*, PlaceableArea, TSubclassOf<APickupableObject>, PickupableObject);

/**
 * Delegate broadcast when a previously placed item is retrieved/removed from this area.
 *
 * @param Interactor The actor removing the item.
 * @param PlaceableArea The placeable area actor from which the item was removed.
 * @param PickupableObject The class of the removed item.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, AActor*, Interactor, APlaceableArea*, PlaceableArea, TSubclassOf<APickupableObject>, PickupableObject);

/**
 * Specialized interactable actor representing a pedestal, altar, or socket where specific items can be deposited or retrieved.
 */
UCLASS()
class HOER_API APlaceableArea : public AInteractableObject, public IUnlockableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceableArea();

	virtual bool Interact(AActor* Interactor) override;
	
	/** Broadcast when an item is successfully placed into this area. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnItemPlaced OnItemPlaced;
	
	/** Broadcast when a placed item is picked back up from this area. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnItemRemoved OnItemRemoved;

	// Unlockable interface implementation
	virtual bool IsUnlocked_Implementation() const override;
	virtual void Unlock_Implementation(bool bUnlock) override;
	virtual FName GetKeyName_Implementation() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Unlockable")
	void SetUnlocked(bool bInUnlocked);

protected:

	/** Whether this placeable area is currently unlocked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Unlockable")
	bool bIsUnlocked;

	//Name of the Pickupable Actor placed into this Placeable Actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Unlockable")
	FName KeyName = NAME_None;
	
	/**
	 * Blueprint event triggered when an item is placed into this area.
	 * Override in Blueprints to script puzzle triggers, animations, or door unlocks.
	 *
	 * @param Interactor The actor placing the item.
	 * @param PlaceableArea This placeable area instance.
	 * @param PickupableObjectClass Class of the item placed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Item Place Success"))
	void BP_OnItemPlaced(AActor* Interactor, APlaceableArea* PlaceableArea, TSubclassOf<APickupableObject> PickupableObjectClass);
	
	/**
	 * Blueprint event triggered when a placed item is removed from this area.
	 * Override in Blueprints to reset puzzle states or lock doors.
	 *
	 * @param Interactor The actor taking the item back.
	 * @param PlaceableArea This placeable area instance.
	 * @param PickupableObjectClass Class of the item removed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Item Remove Success"))
	void BP_OnItemRemoved(AActor* Interactor, APlaceableArea* PlaceableArea, TSubclassOf<APickupableObject> PickupableObjectClass);
	
	/** Static mesh component showing a visual representation of the placed item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	/** The specific pickupable item class accepted by this placeable area. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TSubclassOf<APickupableObject> PickupableObject;
	
	virtual void BeginPlay() override;
	void HideMesh() const;
	void ShowMesh() const;
	
	void EmitItemPlacedEvent(AActor* Interactor);
	void EmitItemRemovedEvent(AActor* Interactor);
};
