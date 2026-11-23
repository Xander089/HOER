// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/ObjectInteraction/InteractableInterface.h"

bool IInteractableInterface::Interact_Implementation(AActor* Interactor)
{
	return false;
}

bool IInteractableInterface::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

FText IInteractableInterface::GetInteractionPrompt_Implementation() const
{
	return FText::GetEmpty();
}

bool IInteractableInterface::WasInteractedWith_Implementation() const
{
	return false;
}

void IInteractableInterface::SetInteractedWith_Implementation(const bool bInteracted)
{
}
