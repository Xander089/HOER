#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditRowWidget.generated.h"

struct FHOERCreditPerson;
class UTextBlock;

/**
 * Row widget displaying a credit category heading and its listed member names.
 */
UCLASS()
class HOER_API UCreditRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Formats and populates the category title and contributors on this credit row.
	 *
	 * @param InCategory Title of the department or category.
	 * @param InPeople List of persons belonging to this category.
	 */
	void SetCreditData(const FText& InCategory, const TArray<FHOERCreditPerson>& InPeople) const;

protected:
	/** Bound TextBlock displaying category title. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CategoryText = nullptr;

	/** Bound TextBlock displaying formatted list of names and handles. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NamesText = nullptr;
};
