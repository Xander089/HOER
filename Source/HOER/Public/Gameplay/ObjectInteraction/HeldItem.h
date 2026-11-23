// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "HeldItem.generated.h"

class APickupableObject;

/**
 * Delegate broadcast when an item is picked up into the character's held socket.
 *
 * @param OutHeldItemClass The class of the picked up item.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemPickup, TSubclassOf<APickupableObject>, OutHeldItemClass);

/**
 * Delegate broadcast when a held item is dropped into the world.
 *
 * @param OutHeldItemClass The class of the dropped item.
 * @param PickupableObject The spawned in-world pickupable object actor.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDrop, TSubclassOf<APickupableObject>, OutHeldItemClass, APickupableObject*, PickupableObject);

/**
 * Component attached to the player character representing currently held items,
 * displaying their mesh and managing pickup, drop, and destruction logic.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOER_API UHeldItem : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHeldItem();
	
	/** Broadcast when an item is picked up into hand. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnItemPickup OnItemPickup;
	
	/** Broadcast when the held item is dropped into the world. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnItemDrop OnItemDrop;
	
	/**
	 * Equips a pickupable item into the held slot and sets its static mesh.
	 *
	 * @param PickupableObjectClass Class of the item to hold.
	 * @param NewMesh The static mesh to display in the character's hand.
	 */
	void PickItem(const TSubclassOf<APickupableObject> PickupableObjectClass, UStaticMesh* NewMesh);

	/** Drops the currently held item into the world at the player's feet. */
	void DropItem();

	/** Destroys the currently held item (e.g. when consumed by a puzzle or locked door). */
	void DestroyItem();

	/** Checks whether the player is currently holding an item. */
	bool IsHolding() const;

	/** Sets holding state flag. */
	void SetIsHolding(const bool bInIsHolding);

	/** Returns the class of the currently held item. */
	TSubclassOf<APickupableObject> GetHeldItemClass() const;

	/** Sets the class of the held item. */
	void SetHeldItemClass(const TSubclassOf<APickupableObject>& InHeldItemClass);

protected:
	/** Class of the currently held item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TSubclassOf<APickupableObject> HeldItemClass;
	
	/**
	 * Blueprint event triggered when an item is picked up.
	 *
	 * @param OutHeldItemClass Class of the item picked up.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Item Pickup"))
	void BP_OnItemPickup(TSubclassOf<APickupableObject> OutHeldItemClass);
	
	/**
	 * Blueprint event triggered when an item is dropped.
	 *
	 * @param OutHeldItemClass Class of the item dropped.
	 * @param PickupableObject The spawned actor in the world.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Item Drop"))
	void BP_OnItemDrop(TSubclassOf<APickupableObject> OutHeldItemClass, APickupableObject* PickupableObject);

	void EmitItemPickupEvent();
	void EmitItemDropEvent(APickupableObject* PickupableObject);
	
private:
	bool bIsHolding = false;
};
