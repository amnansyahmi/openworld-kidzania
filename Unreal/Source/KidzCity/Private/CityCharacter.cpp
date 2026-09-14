#include "CityCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CityInteractable.h"
#include "CityProgression.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

ACityCharacter::ACityCharacter()
{
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 460.f;
    GetCharacterMovement()->AirControl = 0.25f;
    GetCharacterMovement()->MaxAcceleration = 1400.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1700.f;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 330.f;
    CameraBoom->SocketOffset = FVector(0.f, 45.f, 65.f);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 12.f;
    CameraBoom->bDoCollisionTest = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->FieldOfView = 85.f;
}
void ACityCharacter::BeginPlay()
{
    Super::BeginPlay(); SetRunning(false);
}
void ACityCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (IsPlayerControlled() && !bCheckpointRestored)
    {
        bCheckpointRestored = true;
        FTransform Checkpoint;
        auto* Progress = GetGameInstance()->GetSubsystem<UCityProgression>();
        if (Progress->ReadCheckpoint(Checkpoint, FName(*UGameplayStatics::GetCurrentLevelName(this, true))))
            TeleportTo(Checkpoint.GetLocation(), Checkpoint.Rotator(), false, false);
    }
}
void ACityCharacter::SetRunning(bool Running) { GetCharacterMovement()->MaxWalkSpeed = Running ? RunSpeed : WalkSpeed; }
void ACityCharacter::Move(float Forward, float Right)
{
    if (!Controller) return;
    const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Forward);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Right);
}
AActor* ACityCharacter::FindInteractable() const
{
    if (!Controller) return nullptr;
    FVector Eye; FRotator Rotation;
    Controller->GetPlayerViewPoint(Eye, Rotation);
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(CityInteraction), false, this);
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Eye, Eye + Rotation.Vector() * (CameraBoom->TargetArmLength + InteractionDistance), ECC_Visibility, Params)) return nullptr;
    AActor* Actor = Hit.GetActor();
    if (!Actor || !Actor->Implements<UCityInteractable>() || FVector::Dist(GetActorLocation(), Hit.ImpactPoint) > InteractionDistance) return nullptr;
    // Camera traces must not allow interaction through walls behind the shoulder camera.
    FHitResult Occlusion;
    const FVector Start = GetActorLocation() + FVector(0, 0, 45);
    if (GetWorld()->LineTraceSingleByChannel(Occlusion, Start, Hit.ImpactPoint, ECC_Visibility, Params) && Occlusion.GetActor() != Actor) return nullptr;
    return Actor;
}
bool ACityCharacter::SaveCheckpoint()
{
    if (!GetCharacterMovement()->IsMovingOnGround()) return false;
    return GetGameInstance()->GetSubsystem<UCityProgression>()->WriteCheckpoint(GetActorTransform(), FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}
