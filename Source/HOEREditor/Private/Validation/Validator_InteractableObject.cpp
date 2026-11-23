// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Validator_InteractableObject.h"

#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "K2Node_BaseMCDelegate.h"
#include "K2Node_EnhancedInputAction.h"
#include "Logging/TokenizedMessage.h"
#include "Misc/UObjectToken.h"
#include "Gameplay/ObjectInteraction/InteractableObject.h"
#include "Misc/DataValidation.h"

UValidator_InteractableObject::UValidator_InteractableObject()
{
    bIsEnabled = true;
}

bool UValidator_InteractableObject::CanValidateAsset_Implementation(
    const FAssetData& InAssetData,
    UObject* InObject,
    FDataValidationContext& InContext) const
{
    const UBlueprint* Blueprint = Cast<UBlueprint>(InObject);
    if (!Blueprint || !Blueprint->GeneratedClass)
    {
        return false;
    }

    // Only validate Blueprints that inherit from AInteractableObject
    return Blueprint->GeneratedClass->IsChildOf(AInteractableObject::StaticClass());
}

EDataValidationResult UValidator_InteractableObject::ValidateLoadedAsset_Implementation(
    const FAssetData& InAssetData,
    UObject* InAsset,
    FDataValidationContext& Context)
{
    const UBlueprint* Blueprint = Cast<UBlueprint>(InAsset);
    if (!Blueprint || !Blueprint->GeneratedClass)
    {
        return EDataValidationResult::NotValidated;
    }

    // Forbidden function designers must NOT call inside the Blueprint graph
    static const FName ForbiddenFunctionName = GET_FUNCTION_NAME_CHECKED(AInteractableObject, Interact);

    TSet<FName> AllowedEventNames;
    TSet<FName> AllowedDelegateNames;

    // Traverse up from the Blueprint's generated class to collect subclass events (Pickupable, Placeable, Pushable, etc.)
    for (const UClass* Class = Blueprint->GeneratedClass; Class && Class != AActor::StaticClass(); Class = Class->GetSuperClass())
    {
        // Only native C++ classes declare the allowed events/delegates. Blueprint classes are skipped because
        // every public BP function gets FUNC_BlueprintEvent and every event dispatcher is BlueprintAssignable.
        if (!Class->HasAnyClassFlags(CLASS_Native))
        {
            continue;
        }

        for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
        {
            if (const UFunction* Func = *FuncIt; Func && Func->HasAnyFunctionFlags(FUNC_BlueprintEvent) && Func->GetFName() != ForbiddenFunctionName)
            {
                AllowedEventNames.Add(Func->GetFName());
            }
        }

        for (TFieldIterator<FMulticastDelegateProperty> PropIt(Class, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
        {
            if (const FMulticastDelegateProperty* DelegateProp = *PropIt; DelegateProp && DelegateProp->HasAnyPropertyFlags(CPF_BlueprintAssignable))
            {
                AllowedDelegateNames.Add(DelegateProp->GetFName());
            }
        }

        if (Class == AInteractableObject::StaticClass())
        {
            break;
        }
    }

    bool bUsedForbiddenInteract = false;
    bool bHasForbiddenNodes = false;
    bool bUsedAllowedEventOrDelegate = false;

    TArray<UEdGraph*> AllGraphs;
    Blueprint->GetAllGraphs(AllGraphs);

    for (UEdGraph* Graph : AllGraphs)
    {
        if (!Graph) continue;

        for (UEdGraphNode* Node : Graph->Nodes)
        {
            if (!Node || !Node->IsNodeEnabled()) continue;

            // Check: Did the designer call Interact() directly inside the Blueprint?
            if (const UK2Node_CallFunction* CallNode = Cast<UK2Node_CallFunction>(Node))
            {
                const UClass* FunctionParentClass = CallNode->FunctionReference.GetMemberParentClass(
                    CallNode->GetBlueprintClassFromNode());

                if (CallNode->FunctionReference.GetMemberName() == ForbiddenFunctionName &&
                    FunctionParentClass && FunctionParentClass->IsChildOf(AInteractableObject::StaticClass()))
                {
                    bUsedForbiddenInteract = true;
                    Context.AddMessage(EMessageSeverity::Error)
                        ->AddToken(FUObjectToken::Create(Blueprint))
                        ->AddToken(FTextToken::Create(FText::FromString(
                            TEXT("calls 'Interact()' directly in its graph. Use the appropriate interaction events or delegates instead."))));
                }
            }
            
            // Check: Did designer placed ANY Enhanced Input Action event in the graph
            if (Node->IsA<UK2Node_EnhancedInputAction>())
            {
                bHasForbiddenNodes = true;
                Context.AddMessage(EMessageSeverity::Error)
                    ->AddToken(FUObjectToken::Create(Blueprint))
                    ->AddToken(FTextToken::Create(FText::FromString(
                        TEXT("uses an Enhanced Input Action node directly in its graph. Interactable objects must use the exposed interaction events or delegates instead."))));
            }

            // Check: Did the designer implement any allowed BlueprintImplementableEvent with connected logic?
            if (const UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node))
            {
                if (const FName EventName = EventNode->EventReference.GetMemberName(); AllowedEventNames.Contains(EventName))
                {
                    if (const UEdGraphPin* ThenPin = EventNode->FindPin(UEdGraphSchema_K2::PN_Then))
                    {
                        if (ThenPin->LinkedTo.Num() != 0)
                        {
                            bUsedAllowedEventOrDelegate = true;
                        }
                    }
                }
            }

            // Check: Did the designer bind/assign any allowed Multicast Delegate?
            if (const UK2Node_BaseMCDelegate* DelegateNode = Cast<UK2Node_BaseMCDelegate>(Node))
            {
                if (AllowedDelegateNames.Contains(DelegateNode->GetPropertyName()))
                {
                    bUsedAllowedEventOrDelegate = true;
                }
            }
        }
    }

    // Check: Allow child Blueprints whose parent Blueprint already implemented one of the allowed events
    if (!bUsedAllowedEventOrDelegate)
    {
        for (const FName& EventName : AllowedEventNames)
        {
            if (const UFunction* ImplementedFunc = Blueprint->GeneratedClass->FindFunctionByName(EventName))
            {
                // Counts only if a *parent* Blueprint implements it. A native owner means nobody implemented it;
                // this Blueprint's own implementation is already covered by Check 2 (which requires connected pins).
                const UClass* OwnerClass = ImplementedFunc->GetOwnerClass();
                if (Cast<UBlueprintGeneratedClass>(OwnerClass) && OwnerClass != Blueprint->GeneratedClass)
                {
                    bUsedAllowedEventOrDelegate = true;
                    break;
                }
            }
        }
    }

    // Subclasses like APushableObject implement their interaction logic natively in C++,
    // so the mandatory Blueprint event requirement only applies to direct AInteractableObject blueprints.
    const UClass* NativeSuperClass = Blueprint->GeneratedClass;
    while (NativeSuperClass && !NativeSuperClass->HasAnyClassFlags(CLASS_Native))
    {
        NativeSuperClass = NativeSuperClass->GetSuperClass();
    }

    const bool bIsDirectInteractable = (NativeSuperClass == AInteractableObject::StaticClass());

    if (const bool bIsAbstractBP = Blueprint->GeneratedClass && Blueprint->GeneratedClass->HasAnyClassFlags(CLASS_Abstract); bIsDirectInteractable && !bIsAbstractBP && !bUsedAllowedEventOrDelegate)
    {
        Context.AddMessage(EMessageSeverity::Error)
            ->AddToken(FUObjectToken::Create(Blueprint))
            ->AddToken(FTextToken::Create(FText::FromString(
                TEXT("does not implement any valid interaction event with connected pins, nor bind to their delegates."))));

        return EDataValidationResult::Invalid;
    }

    if (bUsedForbiddenInteract || bHasForbiddenNodes)
    {
        return EDataValidationResult::Invalid;
    }

    AssetPasses(InAsset);
    return EDataValidationResult::Valid;
}