#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnlockableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UUnlockableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for unlockable world entities (doors, placeable objects)
 */
class HOER_API IUnlockableInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Checks whether this unlockable object is currently unlocked.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Unlockable")
	bool IsUnlocked() const;

	/**
	 * Unlocks or locks this object.
	 * @param bUnlock Whether to unlock (true) or lock (false) the object.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Unlockable")
	void Unlock(const bool bUnlock);

	// stores the name of the actor used as the key to unlock this unlockable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Unlockable")
	FName GetKeyName() const;
};
