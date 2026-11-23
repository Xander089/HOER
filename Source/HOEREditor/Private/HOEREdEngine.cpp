// Fill out your copyright notice in the Description page of Project Settings.


#include "HOEREdEngine.h"

#include "EditorValidatorSubsystem.h"
#include "ISourceControlModule.h"
#include "Misc/DataValidation.h"

void UHOEREdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	// Register state branches
	const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
	{
		ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();
    
		// Order matters. Lower values are lower in the hierarchy, i.e., changes from higher branches automatically notify lower branches.
		// With this paradigm, the higher the branch is, the stabler it is, and has changes manually promoted up.
    
		const TArray<FString> Branches {"origin/develop"};   
		SourceControlProvider.RegisterStateBranches(Branches, TEXT("Content"));
	}
}

bool UHOEREdEngine::CanSavePackage(UPackage* PackageToSave)
{
	if (!Super::CanSavePackage(PackageToSave))
    {
        return false;
    }

    // Ignore null packages, Engine/Plugin packages, or background auto-saves
    if (!PackageToSave || IsAutosaving())
    {
        return true;
    }

    const FString PackageName = PackageToSave->GetName();
    if (!PackageName.StartsWith(TEXT("/Game/")))
    {
        return true;
    }

    const UEditorValidatorSubsystem* ValidatorSubsystem = GetEditorSubsystem<UEditorValidatorSubsystem>();
    if (!ValidatorSubsystem)
    {
        return true;
    }

    TArray<FText> ValidationErrors;
    TArray<FText> ValidationWarnings;

    // Validate the primary asset inside the package being saved (Blueprint, Level, or External Actor)
    if (UObject* AssetToSave = PackageToSave->FindAssetInPackage())
    {
        const EDataValidationResult AssetResult = ValidatorSubsystem->IsObjectValid(
            AssetToSave,
            ValidationErrors,
            ValidationWarnings,
            EDataValidationUsecase::Save);

        if (AssetResult == EDataValidationResult::Invalid)
        {
            FMessageLog("AssetCheck").Open(EMessageSeverity::Error, true);

            const FString ErrorList = FString::JoinBy(ValidationErrors, TEXT("\n- "), [](const FText& Text) { return Text.ToString(); });
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::Format(
                    NSLOCTEXT("HOERValidation", "SaveBlockedAsset", "Cannot save '{0}' due to validation errors:\n\n- {1}"),
                    FText::FromString(AssetToSave->GetName()),
                    FText::FromString(ErrorList)));

            return false; // Hard-blocks writing the file to disk
        }
    }

    // If saving a Level or a World Partition External Actor, also validate the loaded Editor World
    // so pre-existing unmodified PointLights in the level cannot slip through.
    const bool bIsExternalActorPackage = PackageName.StartsWith(TEXT("/Game/__ExternalActors__/"));
    
    // RENAME: Changed EditorWorld to CurrentEditorWorld to prevent C4458 shadowing error
    if (UWorld* CurrentEditorWorld = GetEditorWorldContext().World(); CurrentEditorWorld && (CurrentEditorWorld->GetOutermost() == PackageToSave || bIsExternalActorPackage))
    {
        ValidationErrors.Reset();
        ValidationWarnings.Reset();

        const EDataValidationResult WorldResult = ValidatorSubsystem->IsObjectValid(
            CurrentEditorWorld,
            ValidationErrors,
            ValidationWarnings,
            EDataValidationUsecase::Save);

        if (WorldResult == EDataValidationResult::Invalid)
        {
            FMessageLog("AssetCheck").Open(EMessageSeverity::Error, true);

            const FString ErrorList = FString::JoinBy(ValidationErrors, TEXT("\n- "), [](const FText& Text) { return Text.ToString(); });
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::Format(
                    NSLOCTEXT("HOERValidation", "SaveBlockedWorld", "Cannot save level '{0}' due to validation errors:\n\n- {1}"),
                    FText::FromString(CurrentEditorWorld->GetName()),
                    FText::FromString(ErrorList)));

            return false; // Hard-blocks writing the level/actor to disk
        }
    }

    return true;
}
