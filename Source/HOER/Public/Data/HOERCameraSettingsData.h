// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HOERCameraSettingsData.generated.h"

/**
 * Primary Data Asset defining first-person and third-person camera orientation, FOV, and socket attachments.
 */
UCLASS()
class HOER_API UHOERCameraSettingsData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Bone or socket on the character skeleton to which the camera spring arm attaches. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FName CameraAttachSocket = FName("head");
	
	/** Relative offset offset vector applied to the camera component from its parent socket. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FVector CameraRelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	
	/** Default horizontal field of view angle in degrees. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView = 90.0f;
	
	/** Whether the camera follows the controller's pitch, yaw, and roll orientation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bUsePawnControlRotation = true;
};
