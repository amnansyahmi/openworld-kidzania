#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityInteractable.h"
#include "CityWorldActors.generated.h"
class UStaticMeshComponent;
class ADirectionalLight;
UENUM(BlueprintType)
enum class ECityLocation : uint8 { Hospital, FireStation, Restaurant, Supermarket, Bank };

UCLASS()
class KIDZCITY_API ACityLocationMarker : public AActor
{
    GENERATED_BODY()
public:
    ACityLocationMarker();
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ECityLocation Location = ECityLocation::Hospital;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText DisplayName;
};

UCLASS()
class KIDZCITY_API ACityDoctorStation : public AActor, public ICityInteractable
{
    GENERATED_BODY()
public:
    ACityDoctorStation();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> Mesh;
    virtual FText InteractionLabel_Implementation() const override;
    virtual void Interact_Implementation(APawn* User) override;
    // Presentation hooks: show patient, uniform transition and clinic animation in the derived BP.
    UFUNCTION(BlueprintImplementableEvent) void OnShiftStarted(APawn* User);
};

UCLASS()
class KIDZCITY_API ACityDoor : public AActor, public ICityInteractable
{
    GENERATED_BODY()
public:
    ACityDoor();
    virtual void Tick(float DeltaSeconds) override;
    virtual FText InteractionLabel_Implementation() const override;
    virtual void Interact_Implementation(APawn* User) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> Door;
    UPROPERTY(EditAnywhere, Category="Door") float OpenAngle = 95.f;
    UPROPERTY(EditAnywhere, Category="Door") float Speed = 3.f;
    UFUNCTION(BlueprintCallable) void SetOpen(bool Open) { bOpen = Open; SetActorTickEnabled(true); }
private:
    bool bOpen = false;
};

UCLASS()
class KIDZCITY_API ACityDayNight : public AActor
{
    GENERATED_BODY()
public:
    ACityDayNight();
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time", meta=(ClampMin="0", ClampMax="24")) float Hour = 9.f;
    UPROPERTY(EditAnywhere, Category="Time", meta=(ClampMin="1")) float DayLengthMinutes = 40.f;
    UPROPERTY(EditInstanceOnly, Category="Lighting") TObjectPtr<ADirectionalLight> Sun;
    UPROPERTY(EditInstanceOnly, Category="Lighting") TObjectPtr<ADirectionalLight> Moon;
    UPROPERTY(EditAnywhere, Category="Lighting") float SunLux = 70000.f;
    UPROPERTY(EditAnywhere, Category="Lighting") float MoonLux = 0.4f;
};
