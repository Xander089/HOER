// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "UnlockableInterface.h"
#include "UnlockableObject.generated.h"



/**
 * Actor representing an unlockable object that can be interacted with.
 */
UCLASS()
class HOER_API AUnlockableObject : public AActor, public IUnlockableInterface, public ISavableActorInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Unlockable")
	void SetUnlocked(bool bInUnlocked);
	
	// Unlockable interface implementation
	virtual bool IsUnlocked_Implementation() const override;
	virtual void Unlock_Implementation(const bool bUnlock) override;
	
	// ISavableActorInterface Methods
	virtual void OnActorSaved() override;
	virtual void OnActorLoaded() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Unlockable")
	void BP_OnUnlocked();
	
	
protected:

	/** Whether this unlockable object is currently unlocked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Unlockable")
	bool bIsUnlocked;

};
