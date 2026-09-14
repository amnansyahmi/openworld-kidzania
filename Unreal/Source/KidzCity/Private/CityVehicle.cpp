#include "CityVehicle.h"
#include "CityCharacter.h"
#include "CityProgression.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/GameInstance.h"

UCityFrontWheel::UCityFrontWheel()
{
    WheelRadius = 34.f; WheelWidth = 22.f; MaxSteerAngle = 35.f;
    bAffectedBySteering = true; bAffectedByEngine = false;
    AxleType = EAxleType::Front;
}
UCityRearWheel::UCityRearWheel()
{
    WheelRadius = 34.f; WheelWidth = 22.f;
    bAffectedBySteering = false; bAffectedByEngine = true; bAffectedByHandbrake = true;
    AxleType = EAxleType::Rear;
}
ACityVehicle::ACityVehicle()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 550.f;
    CameraBoom->SocketOffset = FVector(0, 0, 170);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom);
    auto* Movement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
    Movement->WheelSetups.SetNum(4);
    const FName Bones[] = {TEXT("wheel_fl"), TEXT("wheel_fr"), TEXT("wheel_rl"), TEXT("wheel_rr")};
    for (int32 I = 0; I < 4; ++I)
    {
        Movement->WheelSetups[I].WheelClass = I < 2 ? UCityFrontWheel::StaticClass() : UCityRearWheel::StaticClass();
        Movement->WheelSetups[I].BoneName = Bones[I];
    }
    Movement->EngineSetup.MaxTorque = 350.f;
    Movement->EngineSetup.MaxRPM = 6000.f;
    auto* Curve = Movement->EngineSetup.TorqueCurve.GetRichCurve();
    Curve->AddKey(0.f, 0.4f); Curve->AddKey(2000.f, 0.85f); Curve->AddKey(4000.f, 1.f); Curve->AddKey(6000.f, 0.7f);
}
FText ACityVehicle::InteractionLabel_Implementation() const { return FText::FromString(Driver ? TEXT("Vehicle occupied") : TEXT("Enter vehicle")); }
void ACityVehicle::Interact_Implementation(APawn* User)
{
    auto* Character = Cast<ACityCharacter>(User);
    auto* Player = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
    if (!Player || Driver || !GetMesh()->GetSkeletalMeshAsset() || FVector::Dist(Character->GetActorLocation(), GetActorLocation()) > 400.f) return;
    if (GetGameInstance()->GetSubsystem<UCityProgression>()->IsJobActive()) return;
    Driver = Character; EntryTransform = Character->GetActorTransform();
    Driver->SetRunning(false);
    Driver->GetCharacterMovement()->StopMovementImmediately();
    Driver->GetCharacterMovement()->DisableMovement();
    Driver->SetActorEnableCollision(false);
    Driver->SetActorHiddenInGame(true);
    Driver->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
    Player->Possess(this);
    Player->SetControlRotation(GetActorRotation());
    Handbrake(false);
}
void ACityVehicle::Drive(float Input)
{
    auto* Movement = GetVehicleMovementComponent();
    const float Axis = FMath::Clamp(Input, -1.f, 1.f);
    const float Speed = Movement->GetForwardSpeed();
    const bool ReversingDirection = (Axis < 0 && Speed > 50.f) || (Axis > 0 && Speed < -50.f);
    Movement->SetBrakeInput(ReversingDirection ? FMath::Abs(Axis) : 0.f);
    if (!ReversingDirection)
    {
        if (Axis < 0 && Movement->GetTargetGear() >= 0) Movement->SetTargetGear(-1, true);
        else if (Axis > 0 && Movement->GetTargetGear() <= 0) Movement->SetTargetGear(1, true);
    }
    Movement->SetThrottleInput(ReversingDirection ? 0.f : FMath::Abs(Axis));
}
void ACityVehicle::Steer(float Input) { GetVehicleMovementComponent()->SetSteeringInput(FMath::Clamp(Input, -1.f, 1.f)); }
void ACityVehicle::Handbrake(bool Enabled) { GetVehicleMovementComponent()->SetHandbrakeInput(Enabled); }
bool ACityVehicle::Exit()
{
    if (!Driver || !Controller || FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed()) > 80.f) return false;
    const auto* Capsule = Driver->GetCapsuleComponent();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(CityVehicleExit), false, this); Params.AddIgnoredActor(Driver);
    for (const float Side : {-1.f, 1.f})
    {
        const FVector Candidate = GetActorLocation() + GetActorRightVector() * ExitSideOffset * Side;
        FHitResult Ground;
        if (!GetWorld()->LineTraceSingleByChannel(Ground, Candidate + FVector(0, 0, 250), Candidate - FVector(0, 0, 400), ECC_Visibility, Params) || Ground.ImpactNormal.Z < 0.7f) continue;
        const FVector Position = Ground.ImpactPoint + FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight() + 5.f);
        if (GetWorld()->OverlapBlockingTestByChannel(Position, FQuat::Identity, ECC_Pawn,
            FCollisionShape::MakeCapsule(Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight()), Params)) continue;
        RestoreDriver(Position);
        return true;
    }
    return false; // Both doors blocked: keep possession instead of placing the player inside geometry.
}
void ACityVehicle::RestoreDriver(const FVector& Location)
{
    auto* Player = Cast<APlayerController>(Controller);
    if (!Driver || !Player) return;
    Drive(0); Steer(0); Handbrake(true);
    Driver->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Driver->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
    Driver->SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
    Driver->SetActorHiddenInGame(false); Driver->SetActorEnableCollision(true);
    Driver->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    Player->Possess(Driver);
    Driver->SaveCheckpoint();
    Driver = nullptr;
}
void ACityVehicle::EndPlay(const EEndPlayReason::Type Reason)
{
    if (Reason == EEndPlayReason::Destroyed && Driver) RestoreDriver(EntryTransform.GetLocation());
    Super::EndPlay(Reason);
}
