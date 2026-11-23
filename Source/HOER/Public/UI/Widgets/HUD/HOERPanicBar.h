// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Common/HOERBaseWidget.h"
#include "HOERPanicBar.generated.h"

class UPanicReceiverComponent;
class UProgressBar;

/**
 * HUD progress bar widget visualizing the player's current panic level in real-time.
 */
UCLASS()
class HOER_API UHOERPanicBar : public UHOERBaseWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	/** Bound progress bar reflecting the normalized panic level. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PanicProgressBar = nullptr;
	
private:
	void SetPanicBarProgressValue(float InPanicValue, float InMinPanic, float InMaxPanic) const;

	void SetBindings();
	void ResetBindings();

	UFUNCTION()
	void OnPanicChanged(const float InPanicValue);
	
	TWeakObjectPtr<UPanicReceiverComponent> PanicReceiverComponent = nullptr;
};
