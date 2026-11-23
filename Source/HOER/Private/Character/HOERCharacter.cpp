// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/HOERCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Data/HOERCameraSettingsData.h"
#include "Audio/CharacterAudioComponent.h"
#include "Controller/HOERPlayerController.h"
#include "Data/HOERCharacterSettingsData.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/ObjectInteraction/InteractableObject.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Gameplay/ObjectInteraction/GrabAbilityComponent.h"
#include "InputAction.h"
#include "Character/CompanionCharacter.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "Gameplay/ObjectInteraction/HeldItem.h"
#include "Gameplay/ObjectInteraction/InteractionComponent.h"
#include "Gameplay/PanicSystem/PanicReceiverComponent.h"
#include "System/HOERGameMode.h"

AHOERCharacter::AHOERCharacter()
{
    CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    CameraSpringArm->SetupAttachment(GetMesh(), FName("head"));
    CameraSpringArm->TargetArmLength = 0.0f;
    CameraSpringArm->bDoCollisionTest = false; // Prevent camera from jumping forward near walls
    CameraSpringArm->bUsePawnControlRotation = true; // Let the Spring Arm handle mouse rotation
    CameraSpringArm->bInheritPitch = true;
    CameraSpringArm->bInheritYaw = true;
    CameraSpringArm->bInheritRoll = true;
    CameraSpringArm->SetRelativeLocation(FVector(0.0f, -21.0f, 6.0f));
    CameraSpringArm->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
    FirstPersonCamera->SetRelativeLocation(FVector::ZeroVector);
    FirstPersonCamera->bUsePawnControlRotation = false;
    
    HeldItem = CreateDefaultSubobject<UHeldItem>(TEXT("HeldItem"));
    HeldItem->SetupAttachment(FirstPersonCamera);
    HeldItem->SetRelativeLocation(DefaultHeldItemRelativeLocation);
    
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    
    GrabAbilityComponent = CreateDefaultSubobject<UGrabAbilityComponent>(TEXT("GrabAbilityComponent"));
    
    bUseControllerRotationYaw = true;
    
    DummyCompanionCharacterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyCompanionMesh"));
    DummyCompanionCharacterMesh->SetupAttachment(GetMesh(), RightHandCompanionSocketName);
    DummyCompanionCharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    TrajectoryISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TrajectoryISM"));
    TrajectoryISM->SetupAttachment(GetMesh(), RightHandCompanionSocketName);
    TrajectoryISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TrajectoryISM->SetCastShadow(false);
    TrajectoryISM->SetVisibility(false); // Hidden by default

    GetCharacterMovement()->bOrientRotationToMovement = false;

    CharacterAudioComponent = CreateDefaultSubobject<UCharacterAudioComponent>(TEXT("AudioComponent"));
    
    PanicReceiverComponent = CreateDefaultSubobject<UPanicReceiverComponent>(TEXT("PanicReceiverComponent"));
    
    PanicReceiverComponent->OnPanicValueChanged.AddDynamic(this, &AHOERCharacter::OnPanicChanged);
    
    PanicPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PanicPostProcess"));
    PanicPostProcess->SetupAttachment(RootComponent);
    PanicPostProcess->bUnbound = true; // Makes it affect the whole screen regardless of bounds
    PanicPostProcess->BlendWeight = 0.0f; // Start completely invisible
    PanicPostProcess->Priority = 10; // Ensure it renders on top of the level's default lighting
    
    PanicPostProcess->Settings.bOverride_VignetteIntensity = true;
    PanicPostProcess->Settings.bOverride_FilmGrainIntensity = true;
    PanicPostProcess->Settings.bOverride_ColorSaturation = true;
    PanicPostProcess->Settings.bOverride_ColorGamma = true;
    PanicPostProcess->Settings.bOverride_ColorGain = true;

    // Apply the Darkening / Static Values
    PanicPostProcess->Settings.VignetteIntensity = 1.0f;
    PanicPostProcess->Settings.FilmGrainIntensity = 0.8f;
    // Color Grading settings use FVector4(R, G, B, Alpha). 
    // Crush all color and light (Values near 0.0 mean pitch black)
    PanicPostProcess->Settings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // 0% Color
    PanicPostProcess->Settings.ColorGamma = FVector4(0.1f, 0.1f, 0.1f, 1.0f);      // 10% Midtone brightness
    PanicPostProcess->Settings.ColorGain = FVector4(0.02f, 0.02f, 0.02f, 1.0f);    // 2% Highlight brightness
}

void AHOERCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Apply Data Asset settings once at the start of the game
    ApplyCameraSettings();
    
    OnCharacterMovementUpdated.AddDynamic(this, &AHOERCharacter::HandleMovementUpdated);

    if (InteractionComponent)
    {
        InteractionComponent->OnInteractionAttempted.AddDynamic(this, &AHOERCharacter::HandleInteractionAttempted);
    }
}

void AHOERCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (!CharacterSettings)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Character Settings Data Asset on %s"), *GetName());
            return;
        }

        // Bind Interaction
        if (InteractionComponent)
        {
            EnhancedInputComponent->BindAction(CharacterSettings->InteractAction, ETriggerEvent::Started, InteractionComponent.Get(), &UInteractionComponent::Interact);
        }

        // Bind Companion Action
        EnhancedInputComponent->BindAction(CharacterSettings->ThrowCompanionCharacterAction, ETriggerEvent::Started, this, &AHOERCharacter::StartChargingThrowCompanionCharacter);
        EnhancedInputComponent->BindAction(CharacterSettings->ThrowCompanionCharacterAction, ETriggerEvent::Triggered, this, &AHOERCharacter::UpdateChargingThrowCompanionCharacter);
        EnhancedInputComponent->BindAction(CharacterSettings->ThrowCompanionCharacterAction, ETriggerEvent::Completed, this, &AHOERCharacter::ExecuteThrowCompanionCharacter);

    }
}

void AHOERCharacter::ApplyCameraSettings() const
{
    if (CameraSettings && FirstPersonCamera)
    {
        FirstPersonCamera->SetFieldOfView(CameraSettings->FieldOfView);
        FirstPersonCamera->bUsePawnControlRotation = CameraSettings->bUsePawnControlRotation;

        if (CameraSettings->CameraAttachSocket != NAME_None)
        {
            FirstPersonCamera->SetupAttachment(GetMesh(), CameraSettings->CameraAttachSocket);
            FirstPersonCamera->SetRelativeLocation(CameraSettings->CameraRelativeLocation);
        }
    }
}

void AHOERCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (!Controller) return;
    
    if (GrabAbilityComponent && GrabAbilityComponent->IsGrabbing())
    {
        GrabAbilityComponent->ApplyPushInput(MovementVector);
        return;
    }

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, MovementVector.Y);
    AddMovementInput(RightDir, MovementVector.X);
}

void AHOERCharacter::StartChargingThrowCompanionCharacter(const FInputActionValue& Value)
{
    // Case 1: We are holding the plushie. Start charging the throw!
    if (DummyCompanionCharacterMesh->IsVisible())
    {
        bIsChargingThrowCompanionCharacter = true;
        CurrentCompanionCharacterChargeTime = 0.0f;
    } 
    
    // Case 2: We are NOT holding it. Recall it!
    else
    {
        if (SpawnedCompanionCharacter != nullptr)
        {
            SpawnedCompanionCharacter->Destroy();
            SpawnedCompanionCharacter = nullptr; 
        }

        DummyCompanionCharacterMesh->SetVisibility(true);
    }
    
    // Make the ISM visible when charging begins
    if (bIsChargingThrowCompanionCharacter)
    {
        TrajectoryISM->SetVisibility(true);
    }
}

void AHOERCharacter::UpdateChargingThrowCompanionCharacter(const FInputActionInstance& Instance)
{
    if (!bIsChargingThrowCompanionCharacter) return;

    // Calculate Charge Ratio
    // We add DeltaTime to the charge. Pull the exact active time directly from the Enhanced Input Action Instance
    CurrentCompanionCharacterChargeTime = Instance.GetTriggeredTime();
    float ChargeAlpha = FMath::Clamp(CurrentCompanionCharacterChargeTime / CharacterSettings->MaxChargeTime, 0.0f, 1.0f);

    // Calculate Force
    float CurrentForce = FMath::Lerp(CharacterSettings->MinThrowForce, CharacterSettings->MaxThrowForce, ChargeAlpha);

    // Calculate Direction (Aim slightly up from camera center)
    FVector StartLocation = DummyCompanionCharacterMesh->GetComponentLocation();
    FVector AimDirection = FirstPersonCamera->GetForwardVector() + (FirstPersonCamera->GetUpVector() * 0.2f);
    AimDirection.Normalize();

    CalculatedThrowCompanionCharacterVelocity = AimDirection * CurrentForce;

    // PREDICT TRAJECTORY (The Golden Standard Way)
    FPredictProjectilePathParams PredictParams;
    PredictParams.StartLocation = StartLocation;
    PredictParams.LaunchVelocity = CalculatedThrowCompanionCharacterVelocity;
    PredictParams.bTraceWithCollision = true;
    PredictParams.ProjectileRadius = 15.0f;
    PredictParams.MaxSimTime = 2.0f;
    PredictParams.DrawDebugType = EDrawDebugTrace::None; // Draws the green arc!
    PredictParams.TraceChannel = ECC_Visibility;
    PredictParams.ActorsToIgnore.Add(this);

    FPredictProjectilePathResult PredictResult;
    UGameplayStatics::PredictProjectilePath(this, PredictParams, PredictResult);
    
    // Clear last frame's dots
    TrajectoryISM->ClearInstances();

    // Prevent infinite loops if the designer accidentally sets Spacing to 0 or less
    const int32 SafeSpacing = FMath::Max(1, TrajectoryPointSpacing);

    // Loop through the predicted points using the designer's Spacing step
    for (int32 i = 0; i < PredictResult.PathData.Num(); i += SafeSpacing)
    {
        FTransform DotTransform;
        DotTransform.SetLocation(PredictResult.PathData[i].Location);
        
        // Apply the designer's Scale variable uniformly to X, Y, and Z
        DotTransform.SetScale3D(FVector(TrajectoryPointScale)); 

        // Add the dot to the world
        TrajectoryISM->AddInstance(DotTransform, true);
    }
}

void AHOERCharacter::ExecuteThrowCompanionCharacter(const FInputActionValue& Value)
{
    if (!bIsChargingThrowCompanionCharacter) return;
    bIsChargingThrowCompanionCharacter = false;
    
    // Instantly hide and clear the dots
    TrajectoryISM->SetVisibility(false);
    TrajectoryISM->ClearInstances();

    // Hide the dummy plushie
    DummyCompanionCharacterMesh->SetVisibility(false);

    // Spawn the real plushie
    if (CharacterSettings->CompanionCharacterClassToSpawn)
    {
        const FVector SpawnLoc = DummyCompanionCharacterMesh->GetComponentLocation();
        const FRotator SpawnRot = DummyCompanionCharacterMesh->GetComponentRotation();
        
        // Force the spawn even if it is colliding with the player's body
        SpawnedCompanionCharacter = GetWorld()->SpawnActorDeferred<ACompanionCharacter>(
            CharacterSettings->CompanionCharacterClassToSpawn,
            FTransform(SpawnRot, SpawnLoc),
            this,                                             // Owner
            this,                                             // Instigator
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn   // Collision Override
        );

        if (SpawnedCompanionCharacter)
        {
            SpawnedCompanionCharacter->SetThrower(this);
            // Tell the physics engine that these two actors should ignore each other!
            SpawnedCompanionCharacter->MoveIgnoreActorAdd(this);
            this->MoveIgnoreActorAdd(SpawnedCompanionCharacter);

            // Finish Spawning
            UGameplayStatics::FinishSpawningActor(SpawnedCompanionCharacter, FTransform(SpawnRot, SpawnLoc));
            // Launch it
            SpawnedCompanionCharacter->LaunchCharacter(CalculatedThrowCompanionCharacterVelocity, true, true);
        }
    }
    
    if (OnThrowing.IsBound())
    {
        OnThrowing.Broadcast();
    }
}

void AHOERCharacter::HandleInteractionAttempted(AActor* TargetActor)
{
    if (OnGrabbing.IsBound())
    {
        OnGrabbing.Broadcast();
    }
}

void AHOERCharacter::HandleMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
    if (!GetCharacterMovement()) return;

    if (GetCharacterMovement()->IsFalling())
    {
        TransitionToState(EPlayerState::Falling);
    }
    // Acceleration tracks the player's physical input intent, preventing them from being "Moving" when stuck on a wall
    else if (GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > KINDA_SMALL_NUMBER)
    {
        TransitionToState(EPlayerState::Moving);
    }
    else
    {
        TransitionToState(EPlayerState::Idle);
    }
}

void AHOERCharacter::OnActorSaved()
{
    ISavableActorInterface::OnActorSaved();
}

void AHOERCharacter::OnActorLoaded()
{
    ISavableActorInterface::OnActorLoaded();
}

void AHOERCharacter::TransitionToState(const EPlayerState NewState)
{
    if (NewState == CurrentState) return;

    OnExitState(CurrentState);
    CurrentState = NewState;
    OnEnterState(CurrentState);
}

void AHOERCharacter::OnEnterState(const EPlayerState State) const
{
    switch (State)
    {
        case EPlayerState::Idle: break;
        case EPlayerState::Moving:
            if (OnStartMoving.IsBound()) OnStartMoving.Broadcast(); 
            break;
        case EPlayerState::Falling: break; // Jump sound is handled by OnJumped, not by falling
    }
}

void AHOERCharacter::OnExitState(const EPlayerState State) const
{
    switch (State)
    {
        case EPlayerState::Idle: break;
        case EPlayerState::Moving:
            if (OnStopMoving.IsBound()) OnStopMoving.Broadcast(); 
            break;
        case EPlayerState::Falling: break;
    }
}

void AHOERCharacter::SwitchCharacter(const FInputActionValue& Value)
{
    if (IsValid(SpawnedCompanionCharacter) && SpawnedCompanionCharacter->HasLanded())
    {
        
        Super::SwitchCharacter(Value);
        
        if (IsTooFarFromCharacter(SpawnedCompanionCharacter, CharacterSettings->MaxCompanionLeashDistance))
        {
            return;
        }
        
        if (AHOERPlayerController* PC = GetPlayerController())
        {
            PC->Possess(SpawnedCompanionCharacter);
        }
    }
}

void AHOERCharacter::StopMove(const FInputActionValue& Value)
{
    if (GrabAbilityComponent && GrabAbilityComponent->IsGrabbing())
    {
        GrabAbilityComponent->StopPushInput();
    }
    
}

void AHOERCharacter::OnPanicChanged(const float InPanicValue)
{
    if (!PanicReceiverComponent || !CharacterSettings) return;

    UCharacterMovementComponent* PlayerCharacterMovement = GetCharacterMovement();
    if (!PlayerCharacterMovement) return;

    const float PanicMaxLevel = PanicReceiverComponent->GetMaxPanicLevel();

    if(FMath::IsNearlyEqual(InPanicValue, PanicMaxLevel))
    {
        OnPanicMaxReached();
        return;
    }
    
    const float Threshold = PanicMaxLevel * (CharacterSettings->SlowMovementPanicValuePercentageThreshold / 100.0f);
    
    // Are we panicking enough to slow down?
    if (InPanicValue >= Threshold)
    {
        float Alpha = 0.f;
        
        // Alpha smoothly starts at 0.0 exactly AT the threshold
        // e.g. If Threshold is 50, and Current Panic is 50, Alpha = (50-50)/(100-50) = 0.0
        // e.g. If Threshold is 50, and Current Panic is 75, Alpha = (75-50)/(100-50) = 0.5
        if (PanicMaxLevel > Threshold)
        {
            Alpha = (InPanicValue - Threshold) / (PanicMaxLevel - Threshold);
        }
        
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

        // Movement Penalties
        PlayerCharacterMovement->MaxWalkSpeed = FMath::Lerp(CharacterSettings->MaxWalkSpeed, CharacterSettings->MinPanicWalkSpeed, Alpha);
        PlayerCharacterMovement->JumpZVelocity = FMath::Lerp(CharacterSettings->JumpZVelocity, CharacterSettings->MinPanicJumpZVelocity, Alpha);
        
        // Visual Penalties: Fade in the darkness and static!
        if (PanicPostProcess)
        {
            PanicPostProcess->BlendWeight = Alpha;
        }
    }
    else // If panic drops below the threshold
    {
        // Reset Movement
        PlayerCharacterMovement->MaxWalkSpeed = CharacterSettings->MaxWalkSpeed;
        PlayerCharacterMovement->JumpZVelocity = CharacterSettings->JumpZVelocity;
        
        // Reset Visuals
        if (PanicPostProcess)
        {
            PanicPostProcess->BlendWeight = 0.0f;
        }
    }
}

bool AHOERCharacter::IsInSafeZone() const
{
    return bIsInSafeZone;
}

void AHOERCharacter::SetIsInSafeZone(const bool bInIsInSafeZone)
{
    this->bIsInSafeZone = bInIsInSafeZone;
}


void AHOERCharacter::OnPanicMaxReached() const
{
    if (AHOERGameMode* GM = GetWorld()->GetAuthGameMode<AHOERGameMode>(); IsValid(GM))
    {
        GM->TriggerGameOver(GetController());
    }
}