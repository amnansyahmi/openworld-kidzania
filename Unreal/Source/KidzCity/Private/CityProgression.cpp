#include "CityProgression.h"
#include "Kismet/GameplayStatics.h"

void UCityProgression::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (UGameplayStatics::DoesSaveGameExist(Slot, 0))
    {
        Save = Cast<UCitySaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
        bSaveBlocked = !Save || Save->SchemaVersion != 1 || Save->Coins < 0 || Save->DoctorXP < 0;
        if (!bSaveBlocked)
        {
            TSet<FGuid> Seen;
            int64 Earned = 0;
            for (const FCityReceipt& Receipt : Save->Receipts)
            {
                if (!Receipt.Id.IsValid() || Seen.Contains(Receipt.Id) || Receipt.Coins < 30 || Receipt.Coins > 50 ||
                    Earned > MAX_int64 - Receipt.Coins)
                { bSaveBlocked = true; break; }
                Seen.Add(Receipt.Id);
                Earned += Receipt.Coins;
            }
            if (Earned != Save->Coins || static_cast<int64>(Save->DoctorXP) != static_cast<int64>(Save->Receipts.Num()) * 25)
                bSaveBlocked = true;
        }
    }
    if (!Save) Save = Cast<UCitySaveGame>(UGameplayStatics::CreateSaveGameObject(UCitySaveGame::StaticClass()));
    Status = FText::FromString(bSaveBlocked ? TEXT("Save could not be loaded safely. Existing file preserved; see the save recovery guide.") : TEXT("Visit the hospital to start a Doctor shift."));
}

bool UCityProgression::StartDoctor()
{
    if (bSaveBlocked || !Save || !Session.Start(Save->Receipts.Num() % 2)) return false;
    ActiveReceipt = FGuid::NewGuid();
    Status = FText::FromString(TEXT("Doctor shift started. Listen to your visitor."));
    OnChanged.Broadcast();
    return true;
}

bool UCityProgression::AnswerDoctor(int32 Choice)
{
    if (Session.IsComplete()) return CommitJob(); // Retry a failed disk write without issuing a second receipt.
    const auto Result = Session.Answer(Choice);
    if (Result == KidzCity::Rules::AnswerResult::Inactive) return false;
    if (Result == KidzCity::Rules::AnswerResult::Completed) return CommitJob();
    Status = FText::FromString(Result == KidzCity::Rules::AnswerResult::Wrong ? TEXT("That does not match this visitor's task. Try again.") : TEXT("Good work. Continue the visit."));
    OnChanged.Broadcast();
    return Result == KidzCity::Rules::AnswerResult::Advanced;
}

bool UCityProgression::CommitJob()
{
    if (!Save || bSaveBlocked || !Session.IsComplete() || !ActiveReceipt.IsValid()) return false;
    if (Save->Receipts.ContainsByPredicate([this](const FCityReceipt& R) { return R.Id == ActiveReceipt; })) return false;
    const int32 Reward = Session.Reward();
    if (!KidzCity::Rules::CanCredit(Save->Coins, Save->DoctorXP, Reward)) return false;
    FCityReceipt Receipt;
    Receipt.Id = ActiveReceipt;
    Receipt.Job = FName(UTF8_TO_TCHAR(Session.Definition().Id));
    Receipt.Coins = Reward;
    Save->Coins += Reward;
    Save->DoctorXP += 25;
    Save->Receipts.Add(Receipt);
    if (!Persist())
    {
        Save->Coins -= Reward;
        Save->DoctorXP -= 25;
        Save->Receipts.Pop();
        Status = FText::FromString(TEXT("Could not save reward. Free disk space, then press 1 to retry."));
        OnChanged.Broadcast();
        return false;
    }
    Session.Cancel();
    ActiveReceipt.Invalidate();
    Status = FText::FromString(FString::Printf(TEXT("Shift complete: +%d KidzCoins, +25 Doctor XP."), Reward));
    OnChanged.Broadcast();
    return true;
}

void UCityProgression::CancelJob()
{
    if (!Session.IsActive()) return;
    Session.Cancel(); ActiveReceipt.Invalidate();
    Status = FText::FromString(TEXT("Shift cancelled. No reward issued."));
    OnChanged.Broadcast();
}
int64 UCityProgression::GetCoins() const { return Save && !bSaveBlocked ? Save->Coins : 0; }
int32 UCityProgression::GetDoctorXP() const { return Save && !bSaveBlocked ? Save->DoctorXP : 0; }
FText UCityProgression::GetPrompt() const
{
    return Session.IsActive() ? FText::FromString(UTF8_TO_TCHAR(Session.CurrentStep().Prompt)) : FText::GetEmpty();
}
TArray<FText> UCityProgression::GetChoices() const
{
    TArray<FText> Result;
    if (Session.IsActive()) for (const auto* Choice : Session.CurrentStep().Choices) Result.Add(FText::FromString(UTF8_TO_TCHAR(Choice)));
    return Result;
}
bool UCityProgression::Persist() { return Save && !bSaveBlocked && UGameplayStatics::SaveGameToSlot(Save, Slot, 0); }
bool UCityProgression::WriteCheckpoint(const FTransform& Transform, FName Map)
{
    if (!Save || bSaveBlocked || Transform.ContainsNaN() || Map.IsNone()) return false;
    const FTransform Previous = Save->Checkpoint;
    const FName PreviousMap = Save->CheckpointMap;
    const bool HadCheckpoint = Save->HasCheckpoint;
    Save->Checkpoint = Transform; Save->CheckpointMap = Map; Save->HasCheckpoint = true;
    if (Persist()) return true;
    Save->Checkpoint = Previous; Save->CheckpointMap = PreviousMap; Save->HasCheckpoint = HadCheckpoint;
    return false;
}
bool UCityProgression::ReadCheckpoint(FTransform& Out, FName Map) const
{
    if (!Save || bSaveBlocked || !Save->HasCheckpoint || Save->CheckpointMap != Map || Save->Checkpoint.ContainsNaN()) return false;
    Out = Save->Checkpoint;
    return true;
}
