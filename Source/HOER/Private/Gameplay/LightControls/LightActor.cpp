// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LightControls/LightActor.h"
#include "CollisionDebugDrawingPublic.h"
#include "Components/LocalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "Gameplay/LightControls/ActorLuminanceComponent.h"
#include "System/Subsystem/LightManagerSubsystem.h"

#if !UE_BUILD_SHIPPING
bool GShowLightDebug = false;
static FAutoConsoleVariableRef CVarShowLightsDebug(
	TEXT("HOER.ShowLightDebug"),
	GShowLightDebug,
	TEXT("Show light debug \nOn = 1 \nOff = 0"),
	ECVF_Cheat
);
#endif

// Sets default values
ALightActor::ALightActor()
{
	//This actor will thick, but only if the player is inside the bounds
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	//larger tick interval, we don't need to check every frame
	PrimaryActorTick.TickInterval = 0.5f;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->SetupAttachment(RootComponent);
	SphereCollider->SetCollisionProfileName("Overlap");
	SphereCollider->SetGenerateOverlapEvents(true);
	
	LightComponent = CreateDefaultSubobject<UPointLightComponent>("LightComponent");
	LightComponent->SetupAttachment(RootComponent);
	Intensity = LightComponent->Intensity;
	DefaultIntensity = LightComponent->Intensity;
	Radius = LightComponent->AttenuationRadius;
	SphereCollider->InitSphereRadius(Radius);
	SetInverseFalloffVariable<UPointLightComponent>(LightComponent);
	SetInverseFalloffVariable<USpotLightComponent>(LightComponent);
	bAllowReflection = LightComponent->bAffectReflection;
	IndirectLightingIntensity = LightComponent->IndirectLightingIntensity;
	bOn = LightComponent->Intensity > 0.0;
	
}

void ALightActor::BeginPlay()
{
	Super::BeginPlay();
	
	ULightManagerSubSystem* LightManagerSubSystem = GetWorld()->GetSubsystem<ULightManagerSubSystem>();
	
	if (IsValid(LightManagerSubSystem))
	{
		LightManagerSubSystem->Register(this);
	}
	
}

void ALightActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ULightManagerSubSystem* LightManagerSubSystem = GetWorld()->GetSubsystem<ULightManagerSubSystem>();
	
	if (IsValid(LightManagerSubSystem))
	{
		LightManagerSubSystem->Unregister(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ALightActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!OtherActor)
		return;
	
	ActorsInLight.AddUnique(OtherActor);

#if !UE_BUILD_SHIPPING
	if (IsValid(GEngine) && GShowLightDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Cyan,FString("Enter in light"));
	}
#endif

	SetActorTickEnabled(true);
	
	if (OnStartIlluminateOtherActor.IsBound())
	{
		OnStartIlluminateOtherActor.Broadcast(OtherActor);
	}
}

void ALightActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	ActorsInLight.Remove(OtherActor);
	
	if (UActorLuminanceComponent* Component = OtherActor->FindComponentByClass<UActorLuminanceComponent>())
	{
		Component->RemoveLuminance(this);
		RemoveIndirectLight(OtherActor);
	}
	
#if !UE_BUILD_SHIPPING
	if (IsValid(GEngine) && GShowLightDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Cyan,FString("Exit from light"));
	}
#endif
	if (ActorsInLight.IsEmpty())
		SetActorTickEnabled(false);
	
	if (OnStopIlluminateOtherActor.IsBound())
	{
		OnStopIlluminateOtherActor.Broadcast(OtherActor);
	}
}

// Called every frame
void ALightActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ActorsInLight.IsEmpty())
		return;
	
	for (AActor* Actor  : ActorsInLight)
	{
		GenerateLightTrace(this,Actor, bAllowReflection, Intensity);
	}
	for (const auto& Pair : IndirectActorsInLight)
	{
		if (Pair.Key && Pair.Value.Receiver)
		{
			const float BounceIntensity = Intensity * Pair.Value.CachedReflectance * IndirectLightingIntensity;
			GenerateLightTrace(Pair.Key, Pair.Value.Receiver, false, BounceIntensity);
		}
	}
}
void ALightActor::GenerateLightTrace(AActor* InLightFont, AActor* InLightReceiver, bool bInAllowReflection, const float InIntensity)
{
	FHitResult Hit;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(InLightFont);
	GetWorld()->LineTraceSingleByChannel(Hit,InLightFont->GetActorLocation(),InLightReceiver->GetActorLocation(),LightTraceChannel,CollisionParams);

#if !UE_BUILD_SHIPPING
	if (GShowLightDebug)
	{
		DrawLineTraces(GetWorld(),InLightFont->GetActorLocation(), InLightReceiver->GetActorLocation(),TArray{Hit},5);
	}
#endif

	if (Hit.GetActor() && Hit.GetActor()->FindComponentByClass<UActorLuminanceComponent>())
	{
		if (Hit.GetActor() == InLightReceiver)
		{
			Hit.GetActor()->FindComponentByClass<UActorLuminanceComponent>()->AddLuminance(InLightFont, CalculateLightIntensityFromDistance(InIntensity, Hit.Distance));
		}
	}
	else
	{
		if (UActorLuminanceComponent* Component = InLightReceiver->FindComponentByClass<UActorLuminanceComponent>())
		{
			Component->RemoveLuminance(InLightFont);
			RemoveIndirectLight(InLightReceiver);
		}
	}
	

	if (bInAllowReflection && Hit.GetActor())
	{
		const FVector Dir = InLightReceiver->GetActorLocation() - InLightFont->GetActorLocation();
		const FVector ReflectionVector = FMath::GetReflectionVector(Dir,Hit.ImpactNormal);
		FHitResult ReflectionResult;
		GetWorld()->LineTraceSingleByChannel(ReflectionResult,Hit.ImpactPoint,ReflectionVector * 1000 ,LightTraceChannel,CollisionParams);
#if !UE_BUILD_SHIPPING
		if (GShowLightDebug)
		{
			DrawLineTraces(GetWorld(),Hit.ImpactPoint, ReflectionVector * 1000,TArray{ReflectionResult},5);
		}
#endif
		if (ReflectionResult.GetActor())
		{
			AddIndirectLight(Hit.GetActor(), ReflectionResult.GetActor());
		}	
	}
}

float ALightActor::CalculateLightWithInverseFallOff(const float InIntensity, const float InRadius, const float InDistance) const
{
	return (InIntensity * FMath::Pow(((InRadius - InDistance)/InRadius),2) * (1/(FMath::Pow(FMath::Max(InDistance,1),2))))*1000;
}

float ALightActor::CalculateLightWithLinearFallOff(const float InIntensity,  const float InRadius, const float InDistance, const float InFalloffExponent) const
{
	return (InIntensity*FMath::Pow((1-(InDistance/InRadius)),InFalloffExponent))*1000;
}

float ALightActor::CalculateSurfaceReflectance(const AActor* HitActor) const
{
	if (!HitActor)
	{
		return 0.5f;
	}

	if (const UStaticMeshComponent* Mesh = HitActor->FindComponentByClass<UStaticMeshComponent>())
	{
		if (const UMaterialInterface* Mat = Mesh->GetMaterial(0))
		{
			FLinearColor BaseColor(0.5f, 0.5f, 0.5f, 1.0f);
			Mat->GetVectorParameterValue(FHashedMaterialParameterInfo(TEXT("BaseColor")), BaseColor);
			// Regular physics calculate for luminance
			return BaseColor.R * 0.299f + BaseColor.G * 0.587f + BaseColor.B * 0.114f;
		}
	}

	return 0.5f;
}

float ALightActor::CalculateReflectionIntensity(const AActor* HitActor) const 
{
	return Intensity * CalculateSurfaceReflectance(HitActor) * IndirectLightingIntensity;
}

void ALightActor::AddIndirectLight(AActor* InLightSource, AActor* InLightReceiver)
{
	if (!InLightSource || !InLightReceiver)
	{
		return;
	}

	if (FIndirectLightData* ExistingData = IndirectActorsInLight.Find(InLightSource))
	{
		if (ExistingData->Receiver == InLightReceiver)
		{
			return;
		}

		if (ExistingData->Receiver)
		{
			if (UActorLuminanceComponent* Component = ExistingData->Receiver->FindComponentByClass<UActorLuminanceComponent>())
			{
				Component->RemoveLuminance(InLightSource);
			}
		}

		ExistingData->Receiver = InLightReceiver;
		return;
	}

	const float Reflectance = CalculateSurfaceReflectance(InLightSource);
	IndirectActorsInLight.Add(InLightSource, FIndirectLightData{ InLightReceiver, Reflectance });
}

bool ALightActor::RemoveIndirectLight(AActor* InLightSource)
{
	if (IndirectActorsInLight.IsEmpty() || !InLightSource)
	{
		return false;
	}

	if (FIndirectLightData* ExistingData = IndirectActorsInLight.Find(InLightSource))
	{
		if (ExistingData->Receiver)
		{
			if (UActorLuminanceComponent* Component = ExistingData->Receiver->FindComponentByClass<UActorLuminanceComponent>())
			{
				Component->RemoveLuminance(InLightSource);
			}
		}
		IndirectActorsInLight.Remove(InLightSource);
		return true;
	}

	return false;
}

float ALightActor::CalculateLightIntensityFromDistance(const float InIntensity, const float InDistance)
{
	if (bInverseFallOff)
		return CalculateLightWithInverseFallOff(Intensity,Radius,InDistance);
	
	return CalculateLightWithLinearFallOff(Intensity,Radius,InDistance,FalloffExponent);
}

void ALightActor::ToggleLight_Implementation(bool InbOn)
{
	if (InbOn == bOn)
	{
		return;
	}	
	
	bOn = InbOn;
	
	if (InbOn)
	{
		SetIntensity(DefaultIntensity);
	}
	else
	{
		SetIntensity(0.0f);
	}
	
	if (OnLightStateChanged.IsBound())
	{
		OnLightStateChanged.Broadcast(this);
	}
}

bool ALightActor::IsLightOn_Implementation() const
{
	return bOn;
}

void ALightActor::SetIntensity(float InIntensity)
{
	LightComponent->SetIntensity(InIntensity);
	Intensity = InIntensity;
}


