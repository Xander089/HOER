#include "Gameplay/ObjectInteraction/UnlockableObject.h"


void AUnlockableObject::SetUnlocked(const bool bInUnlocked)
{
	bIsUnlocked = bInUnlocked;
}

bool AUnlockableObject::IsUnlocked_Implementation() const
{
	return bIsUnlocked;
}

void AUnlockableObject::Unlock_Implementation(const bool bUnlock)
{
	bIsUnlocked = bUnlock;
	BP_OnUnlocked();
}

void AUnlockableObject::OnActorSaved()
{
	ISavableActorInterface::OnActorSaved();
}

void AUnlockableObject::OnActorLoaded()
{
	ISavableActorInterface::OnActorLoaded();
}
