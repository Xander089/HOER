// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "HOERButtonWidget.generated.h"

class UTextBlock;
class UButton;
class USizeBox;

/**
 * Delegate broadcast when this custom button widget is clicked or pressed.
 *
 * @param InButton The button widget that was clicked.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, UHOERButtonWidget*, InButton);

/**
 * Delegate broadcast when this button widget receives keyboard or gamepad focus.
 *
 * @param InButton The button widget that received focus.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonFocused, UHOERButtonWidget*, InButton);

/**
 * Reusable menu button widget supporting custom widths, text, gamepad focus navigation,
 * and page-stack visual transitions.
 */
UCLASS()
class HOER_API UHOERButtonWidget : public UHOERBaseWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Sets the text label displayed on the button.
	 *
	 * @param InText The localized text to assign to the button label.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Button")
	void SetButtonText(const FText& InText);

	/**
	 * Sets the width override for the button container.
	 *
	 * @param InWidth Width in pixels/Slate units.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Button")
	void SetButtonWidth(float InWidth);
	
	/** Broadcast when the button is clicked. */
	UPROPERTY(BlueprintAssignable, Category="Game Button|Delegates")
	FOnButtonClicked OnButtonClicked;
	
	/** Broadcast when the button acquires focus. */
	UPROPERTY(BlueprintAssignable, Category="Game Button|Delegates")
	FOnButtonFocused OnButtonFocused;
	
	/** Forces user interface focus onto this button widget. */
	UFUNCTION(BlueprintCallable, Category = "Game Button")
	void SetButtonFocus() const;
	
	/**
	 * Returns the underlying UButton Slate wrapper widget, used to restore focus upon page popping.
	 *
	 * @return Pointer to the inner UWidget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Button")
	UWidget* GetButton() const;
	
	/**
	 * Blueprint event triggered when this button enters the active user focus path.
	 * Override in Blueprints to play focus animations, scale effects, or highlight borders.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Game Button|Style")
	void BP_OnAddedToFocusPath();

	/**
	 * Blueprint event triggered when this button leaves the active user focus path.
	 * Override in Blueprints to reset highlight visuals or animations.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Game Button|Style")
	void BP_OnRemovedFromFocusPath();
	
	/**
	 * Blueprint event triggered when another UI page is pushed onto the stack covering this button's page.
	 * Override in Blueprints to preserve disabled or dimmed appearance.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Game Button|Events")
	void BP_OnPageAddedOnTop();

	/**
	 * Blueprint event triggered when the covering UI page is popped, restoring visibility to this button.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Game Button|Events")
	void BP_OnPageRemovedFromTop();
	
	/** Flags that another page is currently on top of this button's parent page. */
	void SetHasPageOnTop();
	
	/**
	 * Checks whether this button is currently in the active focus path.
	 *
	 * @return True if focused, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Button")
	bool IsFocused();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	
	/** Desired default width of the button size box container. */
	UPROPERTY(EditAnywhere, Category = "Game Button|Config")
	float ButtonWidth = 240.0f;
	
	/** Default text string displayed inside the button label. */
	UPROPERTY(EditAnywhere, Category = "Game Button|Config")
	FText ButtonText;
	
	/** Bound SizeBox controlling button sizing. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category="Game Button")
	TObjectPtr<USizeBox> ButtonSizeBox = nullptr;

	/** Bound UButton widget receiving raw click and hover events. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category="Game Button")
	TObjectPtr<UButton> Button = nullptr;

	/** Optional bound TextBlock displaying the button text label. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Game Button")
	TObjectPtr<UTextBlock> TextBlock = nullptr;
	
	/** Blueprint event fired immediately when the button is pressed. */
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction), Category="Game Button|Config")
	void BP_OnButtonClicked();

private:
	
	UFUNCTION()
	void NativeButtonClicked();
	
	void SetBindings();
	void RemoveBindings() const;
	
	bool bHasPageOnTop = false;
	bool bIsFocused = false;
};
