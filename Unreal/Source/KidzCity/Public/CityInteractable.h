#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CityInteractable.generated.h"
UINTERFACE(BlueprintType)
class KIDZCITY_API UCityInteractable : public UInterface { GENERATED_BODY() };
class KIDZCITY_API ICityInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FText InteractionLabel() const;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Interact(APawn* User);
};
