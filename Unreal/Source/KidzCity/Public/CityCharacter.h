#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CityCharacter.generated.h"
class USpringArmComponent;
class UCameraComponent;
UCLASS()
class KIDZCITY_API ACityCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ACityCharacter();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCameraComponent> FollowCamera;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float WalkSpeed = 260.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement") float RunSpeed = 520.f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction") float InteractionDistance = 260.f;
    UFUNCTION(BlueprintCallable) void SetRunning(bool Running);
    UFUNCTION(BlueprintPure) AActor* FindInteractable() const;
    void Move(float Forward, float Right);
    bool SaveCheckpoint();
protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
private:
    bool bCheckpointRestored = false;
};
