#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CityPedestrian.h"
#include "CityAnimInstance.generated.h"
UCLASS(Transient, Blueprintable)
class KIDZCITY_API UCityAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    UPROPERTY(BlueprintReadOnly, Category="Locomotion") float Speed = 0;
    UPROPERTY(BlueprintReadOnly, Category="Locomotion") float ForwardSpeed = 0;
    UPROPERTY(BlueprintReadOnly, Category="Locomotion") float RightSpeed = 0;
    UPROPERTY(BlueprintReadOnly, Category="Locomotion") bool Falling = false;
    UPROPERTY(BlueprintReadOnly, Category="Locomotion") bool Accelerating = false;
    UPROPERTY(BlueprintReadOnly, Category="Activity") ECityActivity Activity = ECityActivity::Walking;
};
