// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PanicSystem/UPanicProfileDataAsset.h"

#include "Misc/DataValidation.h"

bool UPanicProfileDataAsset::GetStateForLevel(const float PanicLevel, FPanicStateDefinition& OutState) const
{
	for (const FPanicStateDefinition& State : PanicStates)
	{
		if (PanicLevel >= State.Threshold)
		{
			OutState = State;
			return true;
		}
	}
	return false;
}

float UPanicProfileDataAsset::GetMinPanicLevel() const
{
	return PanicStates.Last().Threshold;
}

float UPanicProfileDataAsset::GetMaxPanicLevel() const
{
	return PanicStates[0].Threshold;
}

TArray<FPanicStateDefinition> UPanicProfileDataAsset::GetPanicStates() const
{
	return PanicStates;
}

#if WITH_EDITOR
EDataValidationResult UPanicProfileDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (PanicStates.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("Panic Profile Error: Panic States array is empty. You must define at least one state (e.g., 'Calm' at Threshold 0.0).")));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		// Verify strict descending order (Highest threshold first, lowest last)
		for (int32 i = 1; i < PanicStates.Num(); ++i)
		{
			if (PanicStates[i].Threshold >= PanicStates[i - 1].Threshold)
			{
				Context.AddError(FText::Format(
					FText::FromString(TEXT("Panic Profile Error: States MUST be sorted from highest threshold to lowest. '{0}' ({1}) is improperly placed below '{2}' ({3}).")),
					FText::FromName(PanicStates[i - 1].StateName),
					FText::AsNumber(PanicStates[i - 1].Threshold),
					FText::FromName(PanicStates[i].StateName),
					FText::AsNumber(PanicStates[i].Threshold)
				));
				Result = EDataValidationResult::Invalid;
			}
		}
		
		// Verify there is a baseline state starting at 0.0 to catch total decay
		if (PanicStates.Last().Threshold > 0.0f)
		{
			// Only setting a warning here, not Invalid
			Context.AddError(FText::FromString(TEXT("Panic Profile Warning: Your lowest state threshold is greater than 0.0. The system may not find a valid state when Panic decays entirely.")));
		}
	}

	return Result;
}
#endif
