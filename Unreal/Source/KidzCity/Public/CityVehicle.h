#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "ChaosVehicleWheel.h"
#include "CityInteractable.h"
#include "CityVehicle.generated.h"
class ACityCharacter;
class USpringArmComponent;
class UCameraComponent;
UCLASS()
class KIDZCITY_API UCityFrontWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UCityFrontWheel();
};
UCLASS()
class KIDZCITY_API UCityRearWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UCityRearWheel();
};
UCLASS()
class KIDZCITY_API ACityVehicle : public AWheeledVehiclePawn, public ICityInteractable
{
    GENERATED_BODY()
public:
    ACityVehicle();
    virtual FText InteractionLabel_Implementation() const override;
    virtual void Interact_Implementation(APawn* User) override;
    UFUNCTION(BlueprintCallable) bool Exit();
    void Drive(float Input);
    void Steer(float Input);
    void Handbrake(bool Enabled);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCameraComponent> Camera;
    UPROPERTY(EditAnywhere, Category="Vehicle") float ExitSideOffset = 190.f;
protected:
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    UPROPERTY() TObjectPtr<ACityCharacter> Driver;
    FTransform EntryTransform;
    void RestoreDriver(const FVector& Location);
};
