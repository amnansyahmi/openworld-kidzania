#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "CityPedestrian.generated.h"
class ACityCrossingSignal;
UENUM(BlueprintType)
enum class ECityActivity : uint8 { Walking, Talking, Sitting, Waiting, Shopping, Entering, Exiting, UsingFacility, Crossing, Working, Reacting };
UCLASS()
class KIDZCITY_API ACityActivityPoint : public AActor
{
    GENERATED_BODY()
public:
    ACityActivityPoint();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ECityActivity Activity = ECityActivity::Waiting;
    UPROPERTY(EditAnywhere, meta=(ClampMin="1")) float Duration = 6.f;
    UPROPERTY(EditInstanceOnly) TObjectPtr<ACityCrossingSignal> CrossingSignal;
    // For crossing points: this actor is on the far sidewalk, Entry is on the near sidewalk.
    UPROPERTY(EditAnywhere, meta=(MakeEditWidget=true)) FVector EntryOffset = FVector::ZeroVector;
    bool Reserve(AActor* User);
    void Release(AActor* User);
    bool Available() const { return !ReservedBy.IsValid(); }
    FVector EntryLocation() const { return GetActorTransform().TransformPosition(EntryOffset); }
private:
    TWeakObjectPtr<AActor> ReservedBy;
};
UCLASS()
class KIDZCITY_API ACityPedestrian : public ACharacter
{
    GENERATED_BODY()
public:
    ACityPedestrian();
    UPROPERTY(BlueprintReadOnly) ECityActivity Activity = ECityActivity::Walking;
    void SetActivity(ECityActivity Next);
    UFUNCTION(BlueprintImplementableEvent) void OnActivityChanged(ECityActivity Previous, ECityActivity Next);
};
UCLASS()
class KIDZCITY_API ACityPedestrianController : public AAIController
{
    GENERATED_BODY()
protected:
    virtual void OnPossess(APawn* Pawn) override;
    virtual void OnUnPossess() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
private:
    enum class EPhase { Selecting, Approaching, WaitingForCrossing, Crossing, Performing };
    EPhase Phase = EPhase::Selecting;
    UPROPERTY() TObjectPtr<ACityActivityPoint> Target;
    TWeakObjectPtr<ACityActivityPoint> Previous;
    FTimerHandle BrainTimer;
    float Remaining = 0;
    void Think();
    void ReleaseTarget();
};
