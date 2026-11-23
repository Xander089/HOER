#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicActor.generated.h"

class UPropAudioComponent;

/**
 * World actor managing interactive background music and adjusting multi-layered tension parameters.
 */
UCLASS()
class HOER_API AMusicActor : public AActor
{
	GENERATED_BODY()

public:
	AMusicActor();

	/**
	 * Sets the audio tension level to modulate music intensity between Calm, Mid, and High Tension layers.
	 *
	 * @param InTensionLevel Integer tension indicator (0 = Calm, 1 = Mid, 2+ = Tension/Chase).
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetTensionLevel(int32 InTensionLevel) const;

protected:
	virtual void BeginPlay() override;
	
	/** Prop audio component hosting the interactive MetaSound music player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UPropAudioComponent> PropAudioComponent;
 
	/** Input trigger parameter name for high tension / chase music. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FName TensionHighParameterName = TEXT("Tension");

	/** Input trigger parameter name for medium tension music. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FName TensionMidParameterName = TEXT("Mid");

	/** Input trigger parameter name for low tension / ambient music. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FName TensionLowParameterName = TEXT("Calm");

private:

	void RegisterToAudioSubSystem();
	void UnregisterFromAudioSubSystem();
};
