#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HOERCreditsDataAsset.generated.h"

/**
 * Record holding information for a credited team member or contributor.
 */
USTRUCT(BlueprintType)
struct FHOERCreditPerson
{
	GENERATED_BODY()

	/** First name of the person. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	FString Name;

	/** Last name / surname of the person. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	FString Surname;

	/** GitHub handle or website link. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	FString Github; 
};

/**
 * Credit grouping grouping contributors under a department or discipline header.
 */
USTRUCT(BlueprintType)
struct FHOERCreditCategory
{
	GENERATED_BODY()

	/** Department title (e.g. "Game Developers", "Audio Designers", "3D Art"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	FText CategoryName;

	/** List of contributors belonging to this department. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	TArray<FHOERCreditPerson> People;
};

/**
 * Data Asset containing game attribution and team credits presented on the credits menu screen.
 */
UCLASS()
class HOER_API UHOERCreditsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Ordered list of credit categories and contributors. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	TArray<FHOERCreditCategory> Credits;
};
