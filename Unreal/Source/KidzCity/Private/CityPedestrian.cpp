#include "CityPedestrian.h"
#include "CityTraffic.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
ACityActivityPoint::ACityActivityPoint() { RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root")); }
bool ACityActivityPoint::Reserve(AActor* User)
{
    if (!User || (ReservedBy.IsValid() && ReservedBy.Get() != User)) return false;
    ReservedBy = User; return true;
}
void ACityActivityPoint::Release(AActor* User) { if (ReservedBy.Get() == User) ReservedBy.Reset(); }
ACityPedestrian::ACityPedestrian()
{
    AIControllerClass = ACityPedestrianController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 155.f;
    GetCharacterMovement()->bUseRVOAvoidance = true;
    GetCharacterMovement()->AvoidanceConsiderationRadius = 350.f;
}
void ACityPedestrian::SetActivity(ECityActivity Next)
{
    if (Next == Activity) return;
    const auto Old = Activity; Activity = Next; OnActivityChanged(Old, Next);
}
void ACityPedestrianController::OnPossess(APawn* Pawn)
{
    Super::OnPossess(Pawn);
    GetWorldTimerManager().SetTimer(BrainTimer, this, &ThisClass::Think, 1.f, true, FMath::FRandRange(0.1f, 1.f));
}
void ACityPedestrianController::ReleaseTarget()
{
    auto* OldTarget = Target.Get();
    Target = nullptr;
    Phase = EPhase::Selecting;
    if (IsValid(OldTarget))
    {
        OldTarget->Release(this);
        if (IsValid(OldTarget->CrossingSignal)) OldTarget->CrossingSignal->EndCrossing(this);
        Previous = OldTarget;
    }
    StopMovement();
    ClearFocus(EAIFocusPriority::Gameplay);
}
void ACityPedestrianController::Think()
{
    auto* Citizen = Cast<ACityPedestrian>(GetPawn());
    if (!Citizen) return;
    if (Phase != EPhase::Selecting && !IsValid(Target)) { ReleaseTarget(); return; }
    if (Phase == EPhase::Approaching || Phase == EPhase::Crossing)
    {
        if (--Remaining <= 0)
        {
            if (Phase == EPhase::Crossing)
            {
                // Keep the vehicle stop held while a pedestrian is still on the crossing.
                Remaining = 30;
                MoveToLocation(Target->GetActorLocation(), 40.f, true, true, true, false, nullptr, false);
            }
            else ReleaseTarget();
        }
        return;
    }
    if (Phase == EPhase::WaitingForCrossing)
    {
        if (!IsValid(Target->CrossingSignal)) { ReleaseTarget(); return; }
        if (Target->CrossingSignal->BeginCrossing(this))
        {
            Phase = EPhase::Crossing; Remaining = 30;
            Citizen->SetActivity(ECityActivity::Crossing);
            const auto Result = MoveToLocation(Target->GetActorLocation(), 40.f, true, true, true, false, nullptr, false);
            if (Result == EPathFollowingRequestResult::AlreadyAtGoal) ReleaseTarget();
            else if (Result == EPathFollowingRequestResult::Failed) Remaining = 1;
        }
        return;
    }
    if (Phase == EPhase::Performing)
    {
        if (--Remaining <= 0) ReleaseTarget();
        return;
    }
    // Brief acknowledgement, then resume normal planning; do not continuously chase the player.
    if (auto* Player = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        if (Citizen->Activity != ECityActivity::Reacting && FVector::DistSquared(Player->GetActorLocation(), Citizen->GetActorLocation()) < FMath::Square(160.f))
        { Citizen->SetActivity(ECityActivity::Reacting); SetFocus(Player); return; }
    }
    ClearFocus(EAIFocusPriority::Gameplay);
    TArray<ACityActivityPoint*> Candidates;
    for (TActorIterator<ACityActivityPoint> It(GetWorld()); It; ++It)
        if (It->Available() && *It != Previous.Get() && FVector::DistSquared(It->GetActorLocation(), Citizen->GetActorLocation()) < FMath::Square(5000.f)) Candidates.Add(*It);
    if (Candidates.IsEmpty()) return;
    auto* Selected = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    if (!Selected->Reserve(this)) return;
    Target = Selected; Phase = EPhase::Approaching; Remaining = 45;
    Citizen->SetActivity(ECityActivity::Walking);
    const FVector Destination = Target->Activity == ECityActivity::Crossing ? Target->EntryLocation() : Target->GetActorLocation();
    const auto Result = MoveToLocation(Destination, 45.f, true, true, true, false, nullptr, false);
    if (Result == EPathFollowingRequestResult::Failed) ReleaseTarget();
    else if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
        OnMoveCompleted(FAIRequestID::InvalidRequest, FPathFollowingResult(EPathFollowingResult::Success));
}
void ACityPedestrianController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    if (!IsValid(Target)) return;
    if (!Result.IsSuccess())
    {
        if (Phase == EPhase::Crossing) Remaining = 1; // Retry while retaining the crossing lock.
        else ReleaseTarget();
        return;
    }
    auto* Citizen = Cast<ACityPedestrian>(GetPawn());
    if (!Citizen) { ReleaseTarget(); return; }
    if (Phase == EPhase::Approaching && Target->Activity == ECityActivity::Crossing)
    { Phase = EPhase::WaitingForCrossing; Citizen->SetActivity(ECityActivity::Waiting); return; }
    if (Phase == EPhase::Crossing) { ReleaseTarget(); return; }
    Phase = EPhase::Performing;
    Remaining = FMath::Max(1.f, Target->Duration);
    Citizen->SetActivity(Target->Activity);
    Citizen->SetActorRotation(Target->GetActorRotation());
}
void ACityPedestrianController::OnUnPossess()
{
    GetWorldTimerManager().ClearTimer(BrainTimer); ReleaseTarget(); Super::OnUnPossess();
}
void ACityPedestrianController::EndPlay(const EEndPlayReason::Type Reason)
{
    GetWorldTimerManager().ClearTimer(BrainTimer); ReleaseTarget(); Super::EndPlay(Reason);
}
