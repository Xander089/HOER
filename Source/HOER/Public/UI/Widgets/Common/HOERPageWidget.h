// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "HOERPageWidget.generated.h"

class UPageFooterWidget;
class UFooterDataAsset;
class UHOERButtonWidget;
class UWidgetTree;

/**
 * Base class for full-screen UI pages managed by the AHOERHUD page stack,
 * supporting widget focus retention, footer prompt bindings, and input context switching.
 */
UCLASS()
class HOER_API UHOERPageWidget : public UHOERBaseWidget
{
	GENERATED_BODY()

public:

	/**
	 * Caches the currently focused widget so it can be restored when navigating back.
	 *
	 * @param InWidget The widget that currently holds focus.
	 */
	void CacheFocusedWidget(UWidget* InWidget);
	
	/** Restores focus to the last cached focused widget. */
	void RestoreFocus() const;

	/** Called when this page is pushed onto the HUD stack. */
	virtual void OnPageAdded();

	/** Called when this page is popped and removed from the HUD stack. */
	virtual void OnPageRemoved();

	/** Called when a page on top of this page is popped, restoring this page as active. */
	virtual void OnPageRemovedFromTop();
	
	/**
	 * Updates page footer and prompt icons when gamepad connection status changes.
	 *
	 * @param bIsConnected True if gamepad is connected, false otherwise.
	 */
	virtual void NotifyGamepadConnectionChanged(bool bIsConnected);

	/** Sets whether this page demands the UI input mapping context while active. */
	void SetSwapsInputContext(const bool bInSwapsInputContext) { bSwapsInputContext = bInSwapsInputContext; }

	/** Checks whether this page swaps input mapping context to UI mode. */
	bool SwapsInputContext() const { return bSwapsInputContext; }

	/** Checks whether this page can be closed / popped by pressing the Back / Cancel input. */
	bool CanBeClosedByBack() const { return bCanBeClosedByBack; }

protected:
	
	virtual void CacheDataAssets();

	/** Bound PageFooterWidget instance displaying bottom navigation prompts. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPageFooterWidget> Footer = nullptr;
	
	/** Weak reference to the widget that held focus before this page was covered. */
	UPROPERTY()
	TWeakObjectPtr<UWidget> LastFocusedWidget = nullptr;
	
	/** Map associating specific child widgets with custom footer prompt configurations. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Page")
	TMap<TObjectPtr<UWidget>, TObjectPtr<UFooterDataAsset>> FooterDataMap;

	/** If false, the Back / Escape input is ignored and cannot dismiss this page. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Page")
	bool bCanBeClosedByBack = true;

private:

	/** Whether the UI mapping context must be active while this page is on top of the stack. */
	bool bSwapsInputContext = true;
};
