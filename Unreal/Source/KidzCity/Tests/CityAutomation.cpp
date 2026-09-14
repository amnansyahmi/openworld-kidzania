#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "CityProgression.h"
#include "Kismet/GameplayStatics.h"
#include "Domain/JobRules.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCityDoctorRewardTest, "KidzCity.Careers.DoctorReward", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCityDoctorRewardTest::RunTest(const FString& Parameters)
{
    KidzCity::Rules::DoctorSession Session;
    TestTrue(TEXT("Can start"), Session.Start(0));
    TestFalse(TEXT("Cannot overlap jobs"), Session.Start(1));
    TestEqual(TEXT("No early payout"), Session.Reward(), 0);
    Session.Answer(0);
    TestEqual(TEXT("Wrong answer keeps stage"), Session.StepIndex(), 0);
    for (int I = 0; I < 3; ++I) Session.Answer(Session.CurrentStep().Correct);
    TestEqual(TEXT("Reward reflects one retry"), Session.Reward(), 45);
    TestTrue(TEXT("Completed"), Session.IsComplete());
    Session.Cancel();
    TestEqual(TEXT("Cannot pay cancelled job"), Session.Reward(), 0);
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCitySaveRoundTripTest, "KidzCity.Progression.SaveRoundTrip", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FCitySaveRoundTripTest::RunTest(const FString& Parameters)
{
    auto* Save = NewObject<UCitySaveGame>();
    Save->Coins = 50; Save->DoctorXP = 25;
    FCityReceipt Receipt; Receipt.Id = FGuid::NewGuid(); Receipt.Job = TEXT("doctor.scraped_knee"); Receipt.Coins = 50;
    Save->Receipts.Add(Receipt);
    Save->HasCheckpoint = true; Save->CheckpointMap = TEXT("L_Downtown");
    Save->Checkpoint = FTransform(FVector(100, 200, 96));
    TArray<uint8> Bytes;
    TestTrue(TEXT("Serialize"), UGameplayStatics::SaveGameToMemory(Save, Bytes));
    auto* Loaded = Cast<UCitySaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
    if (!TestNotNull(TEXT("Deserialize"), Loaded)) return false;
    TestEqual(TEXT("Coins preserved"), Loaded->Coins, static_cast<int64>(50));
    TestEqual(TEXT("Receipt preserved"), Loaded->Receipts.Num(), 1);
    if (Loaded->Receipts.Num() == 1) TestTrue(TEXT("Receipt identity preserved"), Loaded->Receipts[0].Id == Receipt.Id);
    TestTrue(TEXT("Checkpoint preserved"), Loaded->Checkpoint.Equals(Save->Checkpoint));
    TestEqual(TEXT("Schema version"), Loaded->SchemaVersion, 1);
    return true;
}
#endif
