#include "CityWorldActors.h"
#include "CityPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "GameFramework/Pawn.h"

ACityLocationMarker::ACityLocationMarker() { RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root")); }
ACityDoctorStation::ACityDoctorStation()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClinicStation")); RootComponent = Mesh;
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}
FText ACityDoctorStation::InteractionLabel_Implementation() const { return FText::FromString(TEXT("Start Doctor shift")); }
void ACityDoctorStation::Interact_Implementation(APawn* User)
{
    if (User) if (auto* Player = Cast<ACityPlayerController>(User->GetController()))
        if (Player->BeginDoctor(this)) OnShiftStarted(User);
}
ACityDoor::ACityDoor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
    Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door")); Door->SetupAttachment(RootComponent);
    Door->SetMobility(EComponentMobility::Movable);
    Door->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
FText ACityDoor::InteractionLabel_Implementation() const { return FText::FromString(bOpen ? TEXT("Close door") : TEXT("Open door")); }
void ACityDoor::Interact_Implementation(APawn* User)
{
    if (User && FVector::Dist(User->GetActorLocation(), GetActorLocation()) < 400.f) SetOpen(!bOpen);
}
void ACityDoor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    const FRotator Target(0, bOpen ? OpenAngle : 0, 0);
    Door->SetRelativeRotation(FMath::RInterpTo(Door->GetRelativeRotation(), Target, DeltaSeconds, Speed));
    if (Door->GetRelativeRotation().Equals(Target, 0.1f)) SetActorTickEnabled(false);
}
ACityDayNight::ACityDayNight() { PrimaryActorTick.bCanEverTick = true; PrimaryActorTick.TickInterval = 0.1f; }
void ACityDayNight::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    Hour = FMath::Fmod(Hour + DeltaSeconds * 24.f / (FMath::Max(1.f, DayLengthMinutes) * 60.f), 24.f);
    const float Elevation = FMath::Sin((Hour - 6.f) / 24.f * 2.f * PI);
    if (Sun)
    {
        Sun->SetActorRotation(FRotator((Hour - 6.f) * -15.f, -30.f, 0.f));
        Sun->GetLightComponent()->SetIntensity(SunLux * FMath::Clamp(Elevation * 3.f, 0.f, 1.f));
    }
    if (Moon)
    {
        Moon->SetActorRotation(FRotator((Hour - 18.f) * -15.f, 150.f, 0.f));
        Moon->GetLightComponent()->SetIntensity(MoonLux * FMath::Clamp(-Elevation * 3.f, 0.f, 1.f));
    }
}
