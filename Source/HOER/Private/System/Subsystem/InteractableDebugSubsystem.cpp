// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/InteractableDebugSubsystem.h"

#include "Components/SphereComponent.h"
#include "Gameplay/ObjectInteraction/InteractableObject.h"

static TAutoConsoleVariable<int32> CVarDebugInteractables(
	TEXT("HOER.DebugInteractables"),
	0,
	TEXT("Draws debug spheres for the Interactable Objects' trigger area.\n")
	TEXT("0: Off\n")
	TEXT("1: On"),
	ECVF_Cheat
);

void UInteractableDebugSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Only do the work if the CVar is enabled
	if (CVarDebugInteractables.GetValueOnGameThread() > 0)
	{
		// Loop backwards so we can safely remove invalid pointers
		for (int32 i = RegisteredInteractables.Num() - 1; i >= 0; --i)
		{
			if (RegisteredInteractables[i].IsValid())
			{
				const AInteractableObject* Obj = RegisteredInteractables[i].Get();
				if (const USphereComponent* Sphere = Obj->GetSphereComponent()) 
				{
					DrawDebugSphere(GetWorld(), Sphere->GetComponentLocation(), Sphere->GetScaledSphereRadius(), 32, FColor::Cyan, false, 0.0f);
				}
			}
			else
			{
				// Clean up dead actors automatically
				RegisteredInteractables.RemoveAt(i);
			}
		}
	}
}

TStatId UInteractableDebugSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UInteractableDebugSubsystem, STATGROUP_Tickables);
}

void UInteractableDebugSubsystem::RegisterInteractable(AInteractableObject* Interactable)
{
	if (Interactable)
	{
		RegisteredInteractables.AddUnique(Interactable);
	}
}

void UInteractableDebugSubsystem::UnregisterInteractable(AInteractableObject* Interactable)
{
	RegisteredInteractables.Remove(Interactable);
}
