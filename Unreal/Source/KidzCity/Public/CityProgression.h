#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Domain/JobRules.h"
#include "CityProgression.generated.h"

USTRUCT(BlueprintType)
struct FCityReceipt
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FGuid Id;
    UPROPERTY(SaveGame, BlueprintReadOnly) FName Job;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 Coins = 0;
};

UCLASS()
class KIDZCITY_API UCitySaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) int32 SchemaVersion = 1;
    UPROPERTY(SaveGame) int64 Coins = 0;
    UPROPERTY(SaveGame) int32 DoctorXP = 0;
    UPROPERTY(SaveGame) TArray<FCityReceipt> Receipts;
    UPROPERTY(SaveGame) bool HasCheckpoint = false;
    UPROPERTY(SaveGame) FTransform Checkpoint;
    UPROPERTY(SaveGame) FName CheckpointMap;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCityProgressChanged);

UCLASS()
class KIDZCITY_API UCityProgression : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    UFUNCTION(BlueprintCallable) bool StartDoctor();
    UFUNCTION(BlueprintCallable) bool AnswerDoctor(int32 Choice);
    UFUNCTION(BlueprintCallable) void CancelJob();
    UFUNCTION(BlueprintPure) int64 GetCoins() const;
    UFUNCTION(BlueprintPure) int32 GetDoctorXP() const;
    UFUNCTION(BlueprintPure) bool IsJobActive() const { return Session.IsActive(); }
    UFUNCTION(BlueprintPure) FText GetPrompt() const;
    UFUNCTION(BlueprintPure) TArray<FText> GetChoices() const;
    UFUNCTION(BlueprintPure) FText GetStatus() const { return Status; }
    UFUNCTION(BlueprintPure) bool CanSave() const { return !bSaveBlocked; }
    UFUNCTION(BlueprintCallable) bool WriteCheckpoint(const FTransform& Transform, FName Map);
    bool ReadCheckpoint(FTransform& Out, FName Map) const;
    UPROPERTY(BlueprintAssignable) FCityProgressChanged OnChanged;
private:
    UPROPERTY() TObjectPtr<UCitySaveGame> Save;
    KidzCity::Rules::DoctorSession Session;
    FGuid ActiveReceipt;
    FText Status;
    bool bSaveBlocked = false;
    bool CommitJob();
    bool Persist();
    static constexpr const TCHAR* Slot = TEXT("KidzCity_Local_v1");
};
