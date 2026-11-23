// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/PanicSystemManager.h"

#include "Gameplay/PanicSystem/PanicEmitterComponent.h"
#include "Gameplay/PanicSystem/PanicReceiverComponent.h"
#include "Settings/PanicSystemSettings.h"

static TAutoConsoleVariable<int32> CVarDebugPanic(
    TEXT("HOER.DebugPanic"),
    0,
    TEXT("Draws debug spheres for the Panic System radii.\n")
    TEXT("0: Off\n")
    TEXT("1: On"),
    ECVF_Cheat
);

void UPanicSystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    const UPanicSystemSettings* Settings = GetDefault<UPanicSystemSettings>();
    const float Interval = Settings ? Settings->UpdateInterval : 1.0f;

    GetWorld()->GetTimerManager().SetTimer(
       PanicUpdateTimerHandle, 
       this, 
       &UPanicSystemManager::ProcessPanicState, 
       Interval, 
       true 
    );
    
}

void UPanicSystemManager::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(PanicUpdateTimerHandle);
    ActiveEmitters.Empty();
    ActiveReceivers.Empty();
    
    Super::Deinitialize();
}

void UPanicSystemManager::RegisterEmitter(UPanicEmitterComponent* Emitter)
{
    if (IsValid(Emitter))
    {
        ActiveEmitters.Add(Emitter);
    }
}

void UPanicSystemManager::UnregisterEmitter(UPanicEmitterComponent* Emitter)
{
    // Do NOT use IsValid() here. If the actor is being destroyed, IsValid returns false,
    // which would prevent us from removing it from the array! Just check for null.
    if (Emitter)
    {
        ActiveEmitters.Remove(Emitter);
    }
}

void UPanicSystemManager::RegisterReceiver(UPanicReceiverComponent* Receiver)
{
    if (IsValid(Receiver))
    {
        ActiveReceivers.Add(Receiver);
    }
}

void UPanicSystemManager::UnregisterReceiver(UPanicReceiverComponent* Receiver)
{
    // Do NOT use IsValid() here. If the actor is being destroyed, IsValid returns false,
    // which would prevent us from removing it from the array! Just check for null.
    if (Receiver)
    {
        ActiveReceivers.Remove(Receiver);
    }
}

void UPanicSystemManager::ProcessPanicState()
{
    const UPanicSystemSettings* Settings = GetDefault<UPanicSystemSettings>();
    const float DeltaTime = Settings ? Settings->UpdateInterval : 1.0f;
    
    for (UPanicReceiverComponent* Receiver : ActiveReceivers)
    {
        if (!IsValid(Receiver))
        {
            continue;
        };
        
        const AActor* ReceiverActor = Receiver->GetOwner();
        if (!ReceiverActor)
        {
            continue;
        };

        float FramePanicAccumulation = 0.f;
        bool bInstantMaxPanic = false;

        for (const UPanicEmitterComponent* Emitter : ActiveEmitters)
        {
            if (!IsValid(Emitter))
            {
                continue;
            };
            
            const AActor* EmitterActor = Emitter->GetOwner();
            if (!EmitterActor)
            {
                continue;
            };
            
            const float Distance = ReceiverActor->GetDistanceTo(EmitterActor);
            const float MinDistance = Emitter->GetMinDistance() * 100.f; 
            const float MaxDistance = Emitter->GetOutOfSightDistance() * 100.f;

            // Enter panic ONLY if in range AND I can see it
            if (Distance <= MaxDistance && Receiver->CanSeeActor(EmitterActor))
            {
                float PanicMultiplier = 0.f;
                
                if (CVarDebugPanic.GetValueOnGameThread() > 0)
                {
                    // Draw Red sphere for MinDistance (Point blank)
                    DrawDebugSphere(GetWorld(), EmitterActor->GetActorLocation(), MinDistance, 32, FColor::Red, false, DeltaTime * 1.5f, 0, 2.0f);
                    // Draw Yellow sphere for OutOfSightDistance (Max range)
                    DrawDebugSphere(GetWorld(), EmitterActor->GetActorLocation(), MaxDistance, 32, FColor::Yellow, false, DeltaTime * 1.5f, 0, 1.0f);
                }

                if (Distance <= MinDistance) {
                    bInstantMaxPanic = true;
                    break;
                } else if (MaxDistance > MinDistance) {
                    const float NormalizedDistance = FMath::Clamp((Distance - MinDistance) / (MaxDistance - MinDistance), 0.0f, 1.0f);
                    PanicMultiplier = 1.0f - NormalizedDistance;
                    if (const UCurveFloat* FalloffCurve = Emitter->GetFalloffCurve()) {
                        PanicMultiplier = FalloffCurve->GetFloatValue(NormalizedDistance);
                    }
                }
                
                // Add this enemy's threat to the total accumulation
                FramePanicAccumulation += Emitter->GetBaseThreat() * PanicMultiplier;
            }
        }
        
        float CurrentPanic = Receiver->GetCurrentPanicLevel();
        
        if (bInstantMaxPanic)
        {
            CurrentPanic = Receiver->GetMaxPanicLevel();
        }
        else
        {
            // Accumulation and Decay fight each other.
            // Close = Accumulation is stronger (Panic goes UP)
            // Far away = Decay is stronger (Panic goes DOWN, even if still in sight!)
            const float NetPanicChange = FramePanicAccumulation - Receiver->GetDecayRate();
            CurrentPanic += NetPanicChange * DeltaTime;
        }

        CurrentPanic = FMath::Clamp(CurrentPanic, 0.f, Receiver->GetMaxPanicLevel());
        Receiver->SetCurrentPanicLevel(CurrentPanic);
    }
}