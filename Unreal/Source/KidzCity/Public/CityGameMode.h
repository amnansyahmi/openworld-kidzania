#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "CityGameMode.generated.h"
UCLASS()
class KIDZCITY_API ACityGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACityGameMode();
};
// Functional native development HUD; replace presentation with UMG in the art pass.
UCLASS()
class KIDZCITY_API ACityHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
