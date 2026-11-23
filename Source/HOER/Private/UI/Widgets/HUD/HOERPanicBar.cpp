// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/HUD/HOERPanicBar.h"

#include "Character/HOERCharacter.h"
#include "Components/ProgressBar.h"
#include "Gameplay/PanicSystem/PanicReceiverComponent.h"
#include "Kismet/GameplayStatics.h"

void UHOERPanicBar::NativeConstruct()
{
	Super::NativeConstruct();
	SetBindings();
}

void UHOERPanicBar::NativeDestruct()
{
	ResetBindings();
	Super::NativeDestruct();
}

void UHOERPanicBar::SetPanicBarProgressValue(const float InPanicValue, const float InMinPanic, const float InMaxPanic) const
{
	const FVector2D PanicMinMaxInput = FVector2D(InMinPanic, InMaxPanic);
	const FVector2D PanicMinMaxOutput = FVector2D(0.f, 1.f);
	const float PanicPercentage = FMath::GetMappedRangeValueClamped(PanicMinMaxInput, PanicMinMaxOutput, InPanicValue);

	if (PanicProgressBar)
	{
		PanicProgressBar->SetPercent(PanicPercentage);
	}
}

void UHOERPanicBar::SetBindings()
{
	const AHOERCharacter* PlayerCharacter = CastChecked<AHOERCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	PanicReceiverComponent = MakeWeakObjectPtr(PlayerCharacter->FindComponentByClass<UPanicReceiverComponent>());
	if (!PanicReceiverComponent.IsValid())
	{
		return;
	}

	SetPanicBarProgressValue(PanicReceiverComponent->GetCurrentPanicLevel(), PanicReceiverComponent->GetMinPanicLevel(),
							  PanicReceiverComponent->GetMaxPanicLevel());

	PanicReceiverComponent->OnPanicValueChanged.AddDynamic(this, &UHOERPanicBar::OnPanicChanged);
}

void UHOERPanicBar::ResetBindings()
{
	if (!PanicReceiverComponent.IsValid())
	{
		return;
	}

	PanicReceiverComponent->OnPanicValueChanged.RemoveDynamic(this, &UHOERPanicBar::OnPanicChanged);
	PanicReceiverComponent.Reset();
}

void UHOERPanicBar::OnPanicChanged(const float InPanicValue)
{
	SetPanicBarProgressValue(InPanicValue, PanicReceiverComponent->GetMinPanicLevel(), PanicReceiverComponent->GetMaxPanicLevel());
}
