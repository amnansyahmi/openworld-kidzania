#include "CityTraffic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
ACityCrossingSignal::ACityCrossingSignal()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    PrimaryActorTick.bCanEverTick = true; PrimaryActorTick.TickInterval = 0.1f;
}
void ACityCrossingSignal::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    Clock = FMath::Fmod(Clock + DeltaSeconds, FMath::Max(1.f, VehicleSeconds + PedestrianSeconds + 2.f * ClearanceSeconds));
    for (auto It = Crossing.CreateIterator(); It; ++It) if (!It->IsValid()) It.RemoveCurrent();
}
bool ACityCrossingSignal::PedestriansMayStart() const
{
    return Clock >= VehicleSeconds + ClearanceSeconds && Clock < VehicleSeconds + ClearanceSeconds + PedestrianSeconds;
}
bool ACityCrossingSignal::VehiclesMayProceed() const { return Clock < VehicleSeconds && Crossing.Num() == 0; }
bool ACityCrossingSignal::BeginCrossing(AActor* Pedestrian)
{
    if (!Pedestrian || !PedestriansMayStart()) return false;
    Crossing.Add(Pedestrian); return true;
}
void ACityCrossingSignal::EndCrossing(AActor* Pedestrian) { Crossing.Remove(Pedestrian); }
ACityTrafficRoute::ACityTrafficRoute()
{
    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Route")); RootComponent = Spline; Spline->SetClosedLoop(true);
}
ACityTrafficVehicle::ACityTrafficVehicle()
{
    PrimaryActorTick.bCanEverTick = true;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Vehicle")); RootComponent = Mesh;
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
void ACityTrafficVehicle::BeginPlay()
{
    Super::BeginPlay();
    if (!Route || !Route->Spline->IsClosedLoop() || Route->Spline->GetSplineLength() <= 1.f)
    { SetActorTickEnabled(false); return; }
    Distance = FMath::Fmod(FMath::Max(0.f, StartDistance), Route->Spline->GetSplineLength());
    SetActorLocationAndRotation(Route->Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World),
        Route->Spline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World));
}
void ACityTrafficVehicle::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!IsValid(Route)) return;
    const float Length = Route->Spline->GetSplineLength();
    if (Length <= 1.f) return;
    // Bound travel during a stall; never jump through the crossing on a very long frame.
    const float Dt = FMath::Min(DeltaSeconds, 0.05f);
    float TargetSpeed = FMath::Max(0.f, CruiseSpeed);
    const float BrakeDistance = Speed * Speed / 1600.f + 250.f;
    const float ProbeDistance = FMath::Max(LookAhead, BrakeDistance);
    for (const auto& Stop : Stops)
    {
        if (!IsValid(Stop.Signal) || Stop.Signal->VehiclesMayProceed()) continue;
        const float Gap = FMath::Fmod(Stop.DistanceOnRoute - Distance + Length, Length);
        if (Gap < ProbeDistance) TargetSpeed = FMath::Min(TargetSpeed, FMath::Sqrt(FMath::Max(0.f, Gap - 200.f) * 1600.f));
    }
    FHitResult Obstacle;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(CityTraffic), false, this);
    const FVector Eye = GetActorLocation() + FVector(0, 0, 70);
    const FVector Ahead = Route->Spline->GetLocationAtDistanceAlongSpline(FMath::Fmod(Distance + ProbeDistance, Length), ESplineCoordinateSpace::World) + FVector(0, 0, 70);
    if (GetWorld()->SweepSingleByChannel(Obstacle, Eye, Ahead, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(65.f), Params))
        TargetSpeed = FMath::Min(TargetSpeed, FMath::Sqrt(FMath::Max(0.f, Obstacle.Distance - 200.f) * 1600.f));
    Speed = FMath::FInterpConstantTo(Speed, TargetSpeed, Dt, TargetSpeed < Speed ? 800.f : 180.f);
    const float Next = FMath::Fmod(Distance + Speed * Dt, Length);
    FHitResult Hit;
    SetActorLocationAndRotation(Route->Spline->GetLocationAtDistanceAlongSpline(Next, ESplineCoordinateSpace::World),
        Route->Spline->GetRotationAtDistanceAlongSpline(Next, ESplineCoordinateSpace::World), true, &Hit);
    if (!Hit.bBlockingHit) Distance = Next;
    else Speed = 0;
}
