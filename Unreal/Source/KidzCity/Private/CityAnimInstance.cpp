#include "CityAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
void UCityAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    auto* Character = Cast<ACharacter>(TryGetPawnOwner());
    if (!Character) return;
    const FVector Velocity = Character->GetVelocity();
    Speed = Velocity.Size2D();
    ForwardSpeed = FVector::DotProduct(Velocity, Character->GetActorForwardVector());
    RightSpeed = FVector::DotProduct(Velocity, Character->GetActorRightVector());
    Falling = Character->GetCharacterMovement()->IsFalling();
    Accelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > 1.f;
    if (auto* Citizen = Cast<ACityPedestrian>(Character)) Activity = Citizen->Activity;
}
