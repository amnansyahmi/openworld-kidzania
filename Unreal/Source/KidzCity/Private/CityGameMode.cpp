#include "CityGameMode.h"
#include "CityCharacter.h"
#include "CityPlayerController.h"
#include "CityProgression.h"
#include "CityInteractable.h"
#include "Engine/Canvas.h"
#include "Engine/GameInstance.h"
ACityGameMode::ACityGameMode()
{
    DefaultPawnClass = ACityCharacter::StaticClass();
    PlayerControllerClass = ACityPlayerController::StaticClass();
    HUDClass = ACityHUD::StaticClass();
}
void ACityHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !PlayerOwner) return;
    auto* Progress = GetGameInstance()->GetSubsystem<UCityProgression>();
    const float Scale = FMath::Clamp(Canvas->ClipY / 1080.f, 0.65f, 1.5f);
    float Y = 28.f * Scale;
    const auto Line = [&](const FString& Text, FLinearColor Color) {
        DrawText(Text, Color, 30.f * Scale, Y, nullptr, Scale * 1.25f);
        Y += 30.f * Scale;
    };
    Line(FString::Printf(TEXT("KidzCoins: %lld   |   Doctor XP: %d"), static_cast<long long>(Progress->GetCoins()), Progress->GetDoctorXP()), FLinearColor::White);
    Line(Progress->GetStatus().ToString(), FLinearColor(0.9f, 0.85f, 0.6f));
    if (Progress->IsJobActive())
    {
        Y += 25.f * Scale;
        Line(Progress->GetPrompt().ToString(), FLinearColor::White);
        const auto Choices = Progress->GetChoices();
        for (int32 Index = 0; Index < Choices.Num(); ++Index)
            Line(FString::Printf(TEXT("%d  %s"), Index + 1, *Choices[Index].ToString()), FLinearColor::White);
        Line(TEXT("X  End shift"), FLinearColor::Gray);
    }
    else if (auto* Character = Cast<ACityCharacter>(PlayerOwner->GetPawn()))
    {
        if (auto* Target = Character->FindInteractable())
            DrawText(TEXT("E  ") + ICityInteractable::Execute_InteractionLabel(Target).ToString(), FLinearColor::White,
                Canvas->ClipX * 0.4f, Canvas->ClipY * 0.76f, nullptr, 1.2f * Scale);
    }
    DrawText(TEXT("WASD Move / Drive    Mouse Look    Shift Run    Space Jump / Brake    E Interact / Exit    F5 Save"),
        FLinearColor::White, 30.f * Scale, Canvas->ClipY - 40.f * Scale, nullptr, Scale);
    DrawRect(FLinearColor::White, Canvas->ClipX * 0.5f - 2, Canvas->ClipY * 0.5f - 2, 4, 4);
}
