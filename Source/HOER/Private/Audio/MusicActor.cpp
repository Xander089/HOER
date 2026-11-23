#include "Audio/MusicActor.h"
#include "Audio/PropAudioComponent.h"
#include "System/Subsystem/AudioSubsystem.h"

AMusicActor::AMusicActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PropAudioComponent = CreateDefaultSubobject<UPropAudioComponent>(TEXT("PropAudioComponent"));
	SetRootComponent(PropAudioComponent);
}

void AMusicActor::SetTensionLevel(const int32 InTensionLevel) const
{
	if (!IsValid(PropAudioComponent))
	{
		return;
	}

	// Ugly solution cause UE5.5 Metasound does not provide Math equality utility nodes 
	switch (InTensionLevel)
	{
		case 0:
		PropAudioComponent->SetFloatParameter(TensionLowParameterName, 1.0f);
		PropAudioComponent->SetFloatParameter(TensionMidParameterName, 0.0f);
		PropAudioComponent->SetFloatParameter(TensionHighParameterName, 0.0f);
		break;

		case 1:
		PropAudioComponent->SetFloatParameter(TensionLowParameterName, 0.0f);
		PropAudioComponent->SetFloatParameter(TensionMidParameterName, 1.0f);
		PropAudioComponent->SetFloatParameter(TensionHighParameterName, 0.0f);
		break;

		case 2:
		PropAudioComponent->SetFloatParameter(TensionLowParameterName, 0.0f);
		PropAudioComponent->SetFloatParameter(TensionMidParameterName, 0.0f);
		PropAudioComponent->SetFloatParameter(TensionHighParameterName, 1.0f);
		break;

		default:
		PropAudioComponent->SetFloatParameter(TensionLowParameterName, 1.0f);
		PropAudioComponent->SetFloatParameter(TensionMidParameterName, 0.0f);
		PropAudioComponent->SetFloatParameter(TensionHighParameterName, 0.0f);
	}
	

}

void AMusicActor::RegisterToAudioSubSystem()
{
	const UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld)) {return; }
	
	const UGameInstance* GI = CurrentWorld->GetGameInstance();
	if (!IsValid(GI)) { return; }
	
	UAudioSubsystem* AudioSubsystem = GI->GetSubsystem<UAudioSubsystem>();
	
	if (!IsValid(AudioSubsystem)) { return;}
	
	AudioSubsystem->RegisterMusicActor(this);
	
}

void AMusicActor::UnregisterFromAudioSubSystem()
{
	const UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld)) {return; }
	
	const UGameInstance* GI = CurrentWorld->GetGameInstance();
	if (!IsValid(GI)) { return; }
	
	UAudioSubsystem* AudioSubsystem = GI->GetSubsystem<UAudioSubsystem>();
	
	if (!IsValid(AudioSubsystem)) { return;}
	
	AudioSubsystem->UnregisterMusicActor(this);
	
}

// Called when the game starts or when spawned
void AMusicActor::BeginPlay()
{
	Super::BeginPlay();
	RegisterToAudioSubSystem();
	
}

