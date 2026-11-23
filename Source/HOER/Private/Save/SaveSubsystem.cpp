// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/SaveSubsystem.h"

#include "EngineUtils.h"
#include "AI/Character/AICharacter.h"
#include "Character/HOERCharacter.h"
#include "Gameplay/LightControls/LightInterface.h"
#include "Gameplay/ObjectInteraction/InteractableInterface.h"
#include "Gameplay/ObjectInteraction/PickupableObject.h"
#include "Gameplay/ObjectInteraction/UnlockableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Save/SavableActorInterface.h"
#include "Save/SaveData.h"

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USaveSubsystem::AsyncSaveGame(const FString& InSaveSlot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UHOERSaveSubsystem::SaveGame"));
    
    FString CurrentSlotName = InSaveSlot.IsEmpty() ? DefaultSaveSlot : InSaveSlot;
    
    if (InSaveSlot.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveGame: Save slot is empty"));
    }
    
    if(CurrentSaveData == nullptr)
    {
        CurrentSaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
        UE_LOG(LogTemp, Log, TEXT("Created new save data object"));
    }

    // Rebuild the per-object state on every save, otherwise objects that were re-locked
    // (or whose item was removed) would keep their stale entries from previous saves
    CurrentSaveData->InteractedInteractables.Empty();
    CurrentSaveData->UnlockedUnlockables.Empty();

    for (AActor* Actor : TActorRange<AActor>(GetWorld()))
    {
        
        if(!IsValid(Actor))
        {
            continue;
        }
        
        if (Actor->Implements<USavableActorInterface>())
        {
            
            if (AHOERCharacter* CurrentCharacter = Cast<AHOERCharacter>(Actor))
            {
                
                if (!IsValid(CurrentCharacter)){ continue;}
                
                CurrentSaveData->PlayerData.ActorName = CurrentCharacter->GetFName();
                CurrentSaveData->PlayerData.Location = CurrentCharacter->GetActorLocation();
                CurrentSaveData->PlayerData.Rotation = CurrentCharacter->GetActorRotation();
                CurrentCharacter->OnActorSaved();
            }
            else if (AAICharacter* AICharacter = Cast<AAICharacter>(Actor))
            {
                if (!IsValid(AICharacter)){ continue;}

                CurrentSaveData->AIEnemyData.ActorName = AICharacter->GetFName();
                CurrentSaveData->AIEnemyData.Location = AICharacter->GetActorLocation();
                CurrentSaveData->AIEnemyData.Rotation = AICharacter->GetActorRotation();
                AICharacter->OnActorSaved();
            }
            else
            {

                if (Actor->Implements<UInteractableInterface>())
                {
                    // store the FActorSaveData for this interactable object if it was interacted with
                    if (IInteractableInterface::Execute_WasInteractedWith(Actor))
                    {
                        FActorSaveData InteractableData;
                        InteractableData.ActorName = Actor->GetFName();
                        InteractableData.Location = Actor->GetActorLocation();
                        InteractableData.Rotation = Actor->GetActorRotation();
                        CurrentSaveData->InteractedInteractables.Add(InteractableData);
                    }
                }

                if (Actor->Implements<UUnlockableInterface>())
                {
                    // store the FActorSaveData for this unlockable object if it is unlocked
                    if (IUnlockableInterface::Execute_IsUnlocked(Actor))
                    {
                        FActorSaveData UnlockableData;
                        UnlockableData.ActorName = Actor->GetFName();
                        UnlockableData.Location = Actor->GetActorLocation();
                        UnlockableData.Rotation = Actor->GetActorRotation();
                        CurrentSaveData->UnlockedUnlockables.Add(UnlockableData);
                    }
                }
            }
            
        }
    }

    FAsyncSaveGameToSlotDelegate SaveDelegate;
    SaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveCompleted);
    UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveData, CurrentSlotName, 0, SaveDelegate);
    
    if (OnWriteGame.IsBound())
    {
        OnWriteGame.Broadcast(CurrentSaveData);
    }
}

void USaveSubsystem::LoadGame(const FString& InSaveSlot)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UHOERSaveSubsystem::LoadGame"));

    FString CurrentSlotName = InSaveSlot.IsEmpty() ? DefaultSaveSlot : InSaveSlot;
    if (InSaveSlot.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Load slot is empty"));
    }

    if(!UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Load slot does not exist"));
        return;
    }

    CurrentSaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
    if (!CurrentSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load save game"));
    }

    if(OnReadGame.IsBound())
    {
        OnReadGame.Broadcast(CurrentSaveData);
    }
}

void USaveSubsystem::OnAsyncSaveCompleted(const FString& SaveSlot, int32 UserIndex, bool bSuccess)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAsyncSaveCompleted Success"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAsyncSaveCompleted Failed"));
    }
}

void USaveSubsystem::RequestLoadOnNextMap()
{
    bPendingLoad = true;
}

void USaveSubsystem::ApplyToWorld(UWorld* InWorld)
{
if (!IsValid(InWorld)  || !bPendingLoad || !IsValid(CurrentSaveData)){ return; }
   
    bPendingLoad = false;
    
    for(AActor* Actor : TActorRange<AActor>(InWorld))
    {
        if(!IsValid(Actor))
        {
            continue;
        }
        
        if (Actor->Implements<USavableActorInterface>())
        {
            
            if (AHOERCharacter* Character = Cast<AHOERCharacter>(Actor)){
                
                if (!IsValid(Character)){ continue;}
                
                Character->SetActorLocation(CurrentSaveData->PlayerData.Location);
                Character->SetActorRotation(CurrentSaveData->PlayerData.Rotation);
                Character->OnActorLoaded();
            }
            else if (AAICharacter* AICharacter = Cast<AAICharacter>(Actor))
            {
                if (!IsValid(AICharacter)){ continue;}

                AICharacter->SetActorLocation(CurrentSaveData->AIEnemyData.Location);
                AICharacter->SetActorRotation(CurrentSaveData->AIEnemyData.Rotation);
                AICharacter->OnActorLoaded();
            }
            else
            {
            
                if (Actor->Implements<UInteractableInterface>())
                {
                    // check if this interactable object was interacted with in the saved game
                    FActorSaveData InteractableData;
                    InteractableData.ActorName = Actor->GetFName();
                    if (const FActorSaveData* SavedData = CurrentSaveData->InteractedInteractables.Find(InteractableData))
                    {
                        Actor->SetActorLocationAndRotation(SavedData->Location, SavedData->Rotation, false, nullptr, ETeleportType::TeleportPhysics);
                        IInteractableInterface::Execute_SetInteractedWith(Actor, true);
                    }
                }

                if (Actor->Implements<UUnlockableInterface>())
                {
                    // check if this unlockable object was unlocked in the saved game
                    FActorSaveData UnlockableData;
                    UnlockableData.ActorName = Actor->GetFName();
                    if (const FActorSaveData* SavedData = CurrentSaveData->UnlockedUnlockables.Find(UnlockableData))
                    {
                        Actor->SetActorLocationAndRotation(SavedData->Location, SavedData->Rotation, false, nullptr, ETeleportType::TeleportPhysics);
                        IUnlockableInterface::Execute_Unlock(Actor, true);

                        // Hides the pickupable actor related to the unlockable actor
                        if (FName KeyName = IUnlockableInterface::Execute_GetKeyName(Actor); KeyName != NAME_None)
                        {
                            if (AActor* FoundKey = FindActorByName(KeyName.ToString()); IsValid(FoundKey))
                            {
                                FoundKey->SetActorHiddenInGame(true);
                                FoundKey->SetActorEnableCollision(false);
                            }
                        }
                    }
                }
            }
            
        }
    }
}

void USaveSubsystem::Register(AActor* InActor)
{
    if (IsValid(InActor))
    {
        FString SaveLabel = InActor->GetName();
        if (const APickupableObject* Pickupable = Cast<APickupableObject>(InActor))
        {
            SaveLabel =  Pickupable->GetSaveLabel();
        }
        ActorReferenceMap.Add(SaveLabel, InActor);
    }
}

void USaveSubsystem::Unregister(const AActor* InActor)
{
    if (IsValid(InActor))
    {
        FString SaveLabel = InActor->GetName();
        if (const APickupableObject* Pickupable = Cast<APickupableObject>(InActor))
        {
            SaveLabel =  Pickupable->GetSaveLabel();
        }
        ActorReferenceMap.Remove(SaveLabel);
    }
}

AActor* USaveSubsystem::FindActorByName(const FString& TargetName)
{

    AActor** ActorPtr = ActorReferenceMap.Find(TargetName);
    if (!ActorPtr)
    {
        return nullptr;
    }

    AActor* Actor = *ActorPtr;
    if (!IsValid(Actor))
    {
        return nullptr;
    }
        
    return Actor;
}
