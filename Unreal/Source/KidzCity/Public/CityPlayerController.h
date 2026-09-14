#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CityPlayerController.generated.h"
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
UCLASS()
class KIDZCITY_API ACityPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ACityPlayerController();
    virtual void PlayerTick(float DeltaSeconds) override;
    bool BeginDoctor(AActor* Station);
protected:
    virtual void SetupInputComponent() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TArray<TObjectPtr<UInputAction>> Actions;
    UPROPERTY() TWeakObjectPtr<AActor> DoctorStation;
    UInputAction* Action(FName Name, bool Axis);
    void Key(UInputAction* Input, FKey Key, bool Negate = false);
    void Forward(const FInputActionValue& Value);
    void Right(const FInputActionValue& Value);
    void LookYaw(const FInputActionValue& Value);
    void LookPitch(const FInputActionValue& Value);
    void JumpStart();
    void JumpStop();
    void RunStart();
    void RunStop();
    void Use();
    void ChoiceOne();
    void ChoiceTwo();
    void ChoiceThree();
    void Answer(int32 Choice);
    void Cancel();
    void Checkpoint();
    void QualityMedium();
    void QualityHigh();
};
