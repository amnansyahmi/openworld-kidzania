#include "CityPlayerController.h"
#include "CityCharacter.h"
#include "CityVehicle.h"
#include "CityInteractable.h"
#include "CityProgression.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameUserSettings.h"

ACityPlayerController::ACityPlayerController() { PrimaryActorTick.bCanEverTick = true; }
UInputAction* ACityPlayerController::Action(FName Name, bool Axis)
{
    auto* Input = NewObject<UInputAction>(this, Name);
    Input->ValueType = Axis ? EInputActionValueType::Axis1D : EInputActionValueType::Boolean;
    Actions.Add(Input);
    return Input;
}
void ACityPlayerController::Key(UInputAction* Input, FKey Code, bool Negate)
{
    auto& Entry = Mapping->MapKey(Input, Code);
    if (Negate) Entry.Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
}
void ACityPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    auto* Enhanced = CastChecked<UEnhancedInputComponent>(InputComponent);
    Mapping = NewObject<UInputMappingContext>(this);
    auto* Fwd = Action(TEXT("Forward"), true); Key(Fwd, EKeys::W); Key(Fwd, EKeys::S, true);
    auto* Side = Action(TEXT("Right"), true); Key(Side, EKeys::D); Key(Side, EKeys::A, true);
    auto* Yaw = Action(TEXT("Yaw"), true); Key(Yaw, EKeys::MouseX);
    auto* Pitch = Action(TEXT("Pitch"), true); Key(Pitch, EKeys::MouseY, true);
    Enhanced->BindAction(Fwd, ETriggerEvent::Triggered, this, &ThisClass::Forward);
    Enhanced->BindAction(Fwd, ETriggerEvent::Completed, this, &ThisClass::Forward);
    Enhanced->BindAction(Fwd, ETriggerEvent::Canceled, this, &ThisClass::Forward);
    Enhanced->BindAction(Side, ETriggerEvent::Triggered, this, &ThisClass::Right);
    Enhanced->BindAction(Side, ETriggerEvent::Completed, this, &ThisClass::Right);
    Enhanced->BindAction(Side, ETriggerEvent::Canceled, this, &ThisClass::Right);
    Enhanced->BindAction(Yaw, ETriggerEvent::Triggered, this, &ThisClass::LookYaw);
    Enhanced->BindAction(Pitch, ETriggerEvent::Triggered, this, &ThisClass::LookPitch);
    auto* Jump = Action(TEXT("Jump"), false); Key(Jump, EKeys::SpaceBar);
    Enhanced->BindAction(Jump, ETriggerEvent::Started, this, &ThisClass::JumpStart);
    Enhanced->BindAction(Jump, ETriggerEvent::Completed, this, &ThisClass::JumpStop);
    auto* Run = Action(TEXT("Run"), false); Key(Run, EKeys::LeftShift);
    Enhanced->BindAction(Run, ETriggerEvent::Started, this, &ThisClass::RunStart);
    Enhanced->BindAction(Run, ETriggerEvent::Completed, this, &ThisClass::RunStop);
    Enhanced->BindAction(Run, ETriggerEvent::Canceled, this, &ThisClass::RunStop);
    const auto Bind = [&](FName Name, FKey Code, void (ThisClass::*Function)()) {
        auto* Input = Action(Name, false); Key(Input, Code);
        Enhanced->BindAction(Input, ETriggerEvent::Started, this, Function);
    };
    Bind(TEXT("Interact"), EKeys::E, &ThisClass::Use);
    Bind(TEXT("Choice1"), EKeys::One, &ThisClass::ChoiceOne);
    Bind(TEXT("Choice2"), EKeys::Two, &ThisClass::ChoiceTwo);
    Bind(TEXT("Choice3"), EKeys::Three, &ThisClass::ChoiceThree);
    Bind(TEXT("CancelJob"), EKeys::X, &ThisClass::Cancel);
    Bind(TEXT("SaveCheckpoint"), EKeys::F5, &ThisClass::Checkpoint);
    Bind(TEXT("MediumQuality"), EKeys::F6, &ThisClass::QualityMedium);
    Bind(TEXT("HighQuality"), EKeys::F7, &ThisClass::QualityHigh);
    if (auto* Local = GetLocalPlayer())
        Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(Mapping, 0);
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}
void ACityPlayerController::Forward(const FInputActionValue& Value)
{
    if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->Move(Value.Get<float>(), 0);
    else if (auto* Vehicle = Cast<ACityVehicle>(GetPawn())) Vehicle->Drive(Value.Get<float>());
}
void ACityPlayerController::Right(const FInputActionValue& Value)
{
    if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->Move(0, Value.Get<float>());
    else if (auto* Vehicle = Cast<ACityVehicle>(GetPawn())) Vehicle->Steer(Value.Get<float>());
}
void ACityPlayerController::LookYaw(const FInputActionValue& Value) { AddYawInput(Value.Get<float>()); }
void ACityPlayerController::LookPitch(const FInputActionValue& Value) { AddPitchInput(Value.Get<float>()); }
void ACityPlayerController::JumpStart()
{
    if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->Jump();
    if (auto* Vehicle = Cast<ACityVehicle>(GetPawn())) Vehicle->Handbrake(true);
}
void ACityPlayerController::JumpStop()
{
    if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->StopJumping();
    if (auto* Vehicle = Cast<ACityVehicle>(GetPawn())) Vehicle->Handbrake(false);
}
void ACityPlayerController::RunStart() { if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->SetRunning(true); }
void ACityPlayerController::RunStop() { if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->SetRunning(false); }
void ACityPlayerController::Use()
{
    if (auto* Vehicle = Cast<ACityVehicle>(GetPawn())) { Vehicle->Exit(); return; }
    if (auto* Character = Cast<ACityCharacter>(GetPawn()))
        if (AActor* Target = Character->FindInteractable()) ICityInteractable::Execute_Interact(Target, Character);
}
bool ACityPlayerController::BeginDoctor(AActor* Station)
{
    if (!Station || !Cast<ACityCharacter>(GetPawn()) || FVector::Dist(GetPawn()->GetActorLocation(), Station->GetActorLocation()) > 400.f) return false;
    if (!GetGameInstance()->GetSubsystem<UCityProgression>()->StartDoctor()) return false;
    DoctorStation = Station;
    return true;
}
void ACityPlayerController::PlayerTick(float DeltaSeconds)
{
    Super::PlayerTick(DeltaSeconds);
    auto* Progress = GetGameInstance()->GetSubsystem<UCityProgression>();
    if (Progress->IsJobActive() && (!DoctorStation.IsValid() || !Cast<ACityCharacter>(GetPawn()) ||
        FVector::Dist(GetPawn()->GetActorLocation(), DoctorStation->GetActorLocation()) > 500.f)) Cancel();
}
void ACityPlayerController::Answer(int32 Choice)
{
    if (DoctorStation.IsValid() && Cast<ACityCharacter>(GetPawn()) &&
        FVector::Dist(GetPawn()->GetActorLocation(), DoctorStation->GetActorLocation()) <= 500.f)
        GetGameInstance()->GetSubsystem<UCityProgression>()->AnswerDoctor(Choice);
}
void ACityPlayerController::ChoiceOne() { Answer(0); }
void ACityPlayerController::ChoiceTwo() { Answer(1); }
void ACityPlayerController::ChoiceThree() { Answer(2); }
void ACityPlayerController::Cancel() { GetGameInstance()->GetSubsystem<UCityProgression>()->CancelJob(); DoctorStation.Reset(); }
void ACityPlayerController::Checkpoint() { if (auto* Character = Cast<ACityCharacter>(GetPawn())) Character->SaveCheckpoint(); }
void ACityPlayerController::QualityMedium()
{
    auto* Settings = UGameUserSettings::GetGameUserSettings();
    Settings->SetOverallScalabilityLevel(1); Settings->SetFrameRateLimit(60); Settings->ApplySettings(false);
}
void ACityPlayerController::QualityHigh()
{
    auto* Settings = UGameUserSettings::GetGameUserSettings();
    Settings->SetOverallScalabilityLevel(2); Settings->SetFrameRateLimit(60); Settings->ApplySettings(false);
}
void ACityPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
    if (auto* Local = GetLocalPlayer()) if (Mapping) Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(Mapping);
    Super::EndPlay(Reason);
}
