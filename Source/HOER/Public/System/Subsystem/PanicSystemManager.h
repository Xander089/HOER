// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PanicSystemManager.generated.h"

class UPanicReceiverComponent;
class UPanicEmitterComponent;

/**
 * World subsystem responsible for tracking all active panic emitters and receivers,
 * periodically evaluating line-of-sight and distances to update panic values.
 */
UCLASS()
class HOER_API UPanicSystemManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
    
	/**
	 * Registers a panic emitter component so its panic influence is processed by the subsystem.
	 *
	 * @param Emitter The panic emitter component to register.
	 */
	UFUNCTION(BlueprintCallable, Category = "Panic System|Management")
	void RegisterEmitter(UPanicEmitterComponent* Emitter);

	/**
	 * Unregisters a panic emitter component from panic state updates.
	 *
	 * @param Emitter The panic emitter component to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category = "Panic System|Management")
	void UnregisterEmitter(UPanicEmitterComponent* Emitter);

	/**
	 * Registers a panic receiver component so it receives panic updates from active emitters.
	 *
	 * @param Receiver The panic receiver component to register.
	 */
	UFUNCTION(BlueprintCallable, Category = "Panic System|Management")
	void RegisterReceiver(UPanicReceiverComponent* Receiver);

	/**
	 * Unregisters a panic receiver component from receiving further panic updates.
	 *
	 * @param Receiver The panic receiver component to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category = "Panic System|Management")
	void UnregisterReceiver(UPanicReceiverComponent* Receiver);

protected:
	/** Collection of currently registered panic emitters in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panic System")
	TSet<UPanicEmitterComponent*> ActiveEmitters;
    
	/** Collection of currently registered panic receivers in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panic System")
	TSet<UPanicReceiverComponent*> ActiveReceivers;

private:
    
	/** Timer handle for running periodic panic state evaluations. */
	FTimerHandle PanicUpdateTimerHandle;
    
	/** Ticking function evaluating proximity and visibility between emitters and receivers. */
	void ProcessPanicState();
};
