// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Validator_LevelLights.h"

#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Light.h"
#include "Gameplay/LightControls/LightActor.h"
#include "Misc/DataValidation.h"
#include "Misc/UObjectToken.h"

UValidator_LevelLights::UValidator_LevelLights()
{
	bIsEnabled = true;
}

bool UValidator_LevelLights::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject,
															 FDataValidationContext& InContext) const
{
	// No logging here: warnings/errors logged during validation are captured and turned into validation errors
	return InObject && (InObject->IsA<UWorld>() || InObject->IsA<ALight>());
}

EDataValidationResult UValidator_LevelLights::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData,
	UObject* InAsset, FDataValidationContext& Context)
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	auto ValidateSingleLight = [&](const ALight* Light)
	{
		if (IsValid(Light) && !Light->IsA<ALightActor>() && !Light->IsA<ADirectionalLight>())
		{
			const TSharedRef<FTokenizedMessage> ErrorMsg = Context.AddMessage(EMessageSeverity::Error);
			ErrorMsg->AddToken(FUObjectToken::Create(Light))
					->AddToken(FTextToken::Create(FText::Format(
						NSLOCTEXT("LightValidation", "ForbiddenLightError",
							"({0}) is a standard Engine Light. Designers must use LightActor instead."),
						FText::FromString(Light->GetClass()->GetName()))));

			Result = EDataValidationResult::Invalid;
		}
	};

	// Case 1: World Partition External Actor save
	if (const ALight* SavedLight = Cast<ALight>(InAsset))
	{
		ValidateSingleLight(SavedLight);
	}

	// Case 2: Traditional Level (.umap) save
	if (const UWorld* World = Cast<UWorld>(InAsset))
	{
		// AllActors: the default OnlyActiveLevels skips invisible levels, i.e. every level of a map that isn't open in the editor
		for (TActorIterator<ALight> It(World, ALight::StaticClass(), EActorIteratorFlags::AllActors | EActorIteratorFlags::SkipPendingKill); It; ++It)
		{
			ValidateSingleLight(*It);
		}
	}

	if (Result == EDataValidationResult::Valid)
	{
		AssetPasses(InAsset);
	}

	return Result;
}
