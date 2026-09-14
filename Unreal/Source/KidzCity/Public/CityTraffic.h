#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityTraffic.generated.h"
class USplineComponent;
class UStaticMeshComponent;
UCLASS()
class KIDZCITY_API ACityCrossingSignal : public AActor
{
    GENERATED_BODY()
public:
    ACityCrossingSignal();
    virtual void Tick(float DeltaSeconds) override;
    UFUNCTION(BlueprintPure) bool PedestriansMayStart() const;
    UFUNCTION(BlueprintPure) bool VehiclesMayProceed() const;
    bool BeginCrossing(AActor* Pedestrian);
    void EndCrossing(AActor* Pedestrian);
    UPROPERTY(EditAnywhere, Category="Traffic", meta=(ClampMin="1")) float VehicleSeconds = 20.f;
    UPROPERTY(EditAnywhere, Category="Traffic", meta=(ClampMin="1")) float PedestrianSeconds = 10.f;
    UPROPERTY(EditAnywhere, Category="Traffic", meta=(ClampMin="1")) float ClearanceSeconds = 3.f;
private:
    float Clock = 0;
    TSet<TWeakObjectPtr<AActor>> Crossing;
};
UCLASS()
class KIDZCITY_API ACityTrafficRoute : public AActor
{
    GENERATED_BODY()
public:
    ACityTrafficRoute();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USplineComponent> Spline;
};
USTRUCT(BlueprintType)
struct FCityTrafficStop
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) TObjectPtr<ACityCrossingSignal> Signal;
    UPROPERTY(EditAnywhere, meta=(ClampMin="0")) float DistanceOnRoute = 0.f;
};
// Background traffic uses swept spline motion. Player vehicles use Chaos physics separately.
UCLASS()
class KIDZCITY_API ACityTrafficVehicle : public AActor
{
    GENERATED_BODY()
public:
    ACityTrafficVehicle();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(EditInstanceOnly) TObjectPtr<ACityTrafficRoute> Route;
    UPROPERTY(EditAnywhere) TArray<FCityTrafficStop> Stops;
    UPROPERTY(EditAnywhere, meta=(ClampMin="0")) float StartDistance = 0;
    UPROPERTY(EditAnywhere, meta=(ClampMin="0")) float CruiseSpeed = 650.f;
    UPROPERTY(EditAnywhere, meta=(ClampMin="50")) float LookAhead = 650.f;
private:
    float Distance = 0;
    float Speed = 0;
};
