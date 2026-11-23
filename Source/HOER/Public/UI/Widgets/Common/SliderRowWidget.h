// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "SliderRowWidget.generated.h"

class USizeBox;
class USlider;
class UTextBlock;

/**
 * Delegate broadcast when the slider row's numeric value is adjusted.
 *
 * @param InSliderValue The new slider floating-point value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderChanged, const float, InSliderValue);

/**
 * Reusable menu slider row widget for settings (such as music volume, SFX volume, look sensitivity),
 * featuring percentage readouts, focus styling, and gamepad navigation.
 */
UCLASS()
class HOER_API USliderRowWidget : public UHOERBaseWidget
{
	GENERATED_BODY()
	
public:
	
	/** Sets focus state on this row widget. */
	void SetFocus(bool IsFocused);
	
	/** Broadcast whenever the slider's value changes. */
	UPROPERTY(BlueprintAssignable, Category="Game Slider|Delegates")
	FOnSliderChanged OnSliderChanged;
	
	/**
	 * Adjusts the overall width of the slider row size box.
	 *
	 * @param InWidth Width in Slate units.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Slider")
	void SetSliderWidth(float InWidth);
	
	/**
	 * Sets the numeric value of the slider programmatically.
	 *
	 * @param InValue The float value to set on the slider (typically 0.0 to 1.0).
	 */
	UFUNCTION(BlueprintCallable, Category="Game Slider")
	void SetSliderValue(float InValue);

	/**
	 * Blueprint event triggered when this slider row acquires focus.
	 * Override in Blueprints to apply glow, color shifts, or sound cues.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Row added to focus Path")
	void BP_OnAddedToFocusPath();

	/**
	 * Blueprint event triggered when this slider row loses focus.
	 *
	 * @param bPlaySound Whether audio feedback should accompany unfocusing.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Row removed from focus Path")
	void BP_OnRemovedFromFocusPath(bool bPlaySound = true);
	
protected:
	UFUNCTION()
	void NativeSliderChanged(float Value);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	
	/** Bound USlider widget component. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USlider> SliderWidget = nullptr;

	/** Bound SizeBox controlling row width. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBoxWidget = nullptr;
	
	/** Bound TextBlock displaying numeric / percentage readout. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueTextWidget = nullptr;
	
	/** Desired default width of the slider row widget. */
	UPROPERTY(EditAnywhere, Category = "Game Slider|Config")
	float SliderRowWidth = 900.0f;

private:
	
	bool bSetInitialKeyboardFocus = false;
	bool bIsSettingValue = false;

	void SetBindings();
	void RemoveBindings() const;
	
	void UpdateValueText(const float Value) const;
};
