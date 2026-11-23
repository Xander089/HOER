// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "HOERCommandsPageWidget.generated.h"

class UCommandRowWidget;

/**
 * Commands and Controls menu page widget illustrating keyboard/mouse and gamepad button bindings.
 */
UCLASS()
class HOER_API UHOERCommandsPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()

protected:

	/** Bound row widget for movement controls. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> MoveRow = nullptr;

	/** Bound row widget for camera look controls. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> LookRow = nullptr;

	/** Bound row widget for jump action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> JumpRow = nullptr;

	/** Bound row widget for sprint action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> SprintRow = nullptr;

	/** Bound row widget for throw companion action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> ThrowCompanionRow = nullptr;

	/** Bound row widget for switch companion character action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> SwitchControlRow = nullptr;

	/** Bound row widget for interaction action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> InteractRow = nullptr;

	/** Bound row widget for UI back / cancel action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> BackRow = nullptr;

	/** Bound row widget for pause action. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommandRowWidget> PauseRow = nullptr;
};
