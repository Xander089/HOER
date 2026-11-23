// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/ObjectInteraction/GrabAbilityComponent.h"
#include "Gameplay/ObjectInteraction/PushableObject.h"

UGrabAbilityComponent::UGrabAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGrabAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGrabAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsGrabbing())
	{
		ReleaseGrab();
	}

	Super::EndPlay(EndPlayReason);
}

bool UGrabAbilityComponent::IsGrabbing() const
{
	return IsValid(GrabbedObject);
}

bool UGrabAbilityComponent::Grab(APushableObject* InObjectToGrab)
{
	if (!IsValid(InObjectToGrab))
	{
		return false;
	}

	if (GrabbedObject == InObjectToGrab)
	{
		return true;
	}

	if (IsValid(GrabbedObject))
	{
		ReleaseGrab();
	}

	GrabbedObject = InObjectToGrab;

	if (OnObjectGrabbed.IsBound())
	{
		OnObjectGrabbed.Broadcast(InObjectToGrab);
	}

	return true;
}

void UGrabAbilityComponent::ReleaseGrab()
{
	if (!GrabbedObject)
	{
		return;
	}

	APushableObject* ObjectToRelease = GrabbedObject;
	GrabbedObject = nullptr;

	if (IsValid(ObjectToRelease))
	{
		ObjectToRelease->StopPushing();
	}

	if (OnObjectReleased.IsBound())
	{
		OnObjectReleased.Broadcast(ObjectToRelease);
	}
}

void UGrabAbilityComponent::ApplyPushInput(const FVector2D& MovementVector)
{
	if (IsValid(GrabbedObject))
	{
		GrabbedObject->ApplyPushInput(MovementVector);
	}
}

void UGrabAbilityComponent::StopPushInput()
{
	ApplyPushInput(FVector2D::ZeroVector);
}

bool UGrabAbilityComponent::TryHandleMovementInput(const FVector2D& MovementVector)
{
	if (IsGrabbing())
	{
		ApplyPushInput(MovementVector);
		return true;
	}

	return false;
}
