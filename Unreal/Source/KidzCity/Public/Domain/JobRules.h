#pragma once
#include <array>
#include <cstdint>
#include <limits>

// Engine-independent rules. The native tests exercise the exact code used by Unreal.
namespace KidzCity::Rules
{
enum class AnswerResult { Inactive, Wrong, Advanced, Completed };
struct Step
{
    const char* Prompt;
    std::array<const char*, 3> Choices;
    int Correct;
};
struct DoctorCase
{
    const char* Id;
    const char* Title;
    std::array<Step, 3> Steps;
};
inline constexpr std::array<DoctorCase, 2> DoctorCases {{
    {"doctor.scraped_knee", "A visitor has a scraped knee", {{
        {"Ask the visitor what happened. What should you check first?",
         {"Choose a bandage immediately", "Listen and inspect the knee", "Send them back to play"}, 1},
        {"The visitor fell while playing. You see a small surface scrape. Choose the clinic tray.",
         {"Clean-up supplies", "An ankle support", "A lunch tray"}, 0},
        {"The scrape is now clean. Finish this pretend clinic task.",
         {"Leave the task unfinished", "Use the ankle support", "Apply a dressing and reassure the visitor"}, 2}
    }}},
    {"doctor.checkup", "A visitor arrives for a routine check-up", {{
        {"The visitor is nervous. How do you start?",
         {"Rush the examination", "Ignore the visitor", "Introduce yourself and explain the check-up"}, 2},
        {"The visit card requests a temperature reading. Choose the tool.",
         {"A thermometer", "A bandage", "A cooking spoon"}, 0},
        {"The reading is recorded on the visit card. Complete the visit.",
         {"Delete the reading", "Explain the completed check-up and thank the visitor", "Start a different job"}, 1}
    }}}
}};

class DoctorSession
{
public:
    bool Start(int CaseIndex)
    {
        if (Active || CaseIndex < 0 || CaseIndex >= static_cast<int>(DoctorCases.size())) return false;
        Case = CaseIndex; Stage = 0; Mistakes = 0; Active = true; Complete = false;
        return true;
    }
    AnswerResult Answer(int Choice)
    {
        if (!Active || Complete || Choice < 0 || Choice > 2) return AnswerResult::Inactive;
        if (Choice != CurrentStep().Correct)
        {
            if (Mistakes < 100) ++Mistakes;
            return AnswerResult::Wrong;
        }
        ++Stage;
        Complete = Stage == 3;
        return Complete ? AnswerResult::Completed : AnswerResult::Advanced;
    }
    void Cancel() { Active = false; Complete = false; Stage = 0; Mistakes = 0; }
    bool IsActive() const { return Active; }
    bool IsComplete() const { return Active && Complete; }
    int StepIndex() const { return Stage; }
    int CaseIndex() const { return Case; }
    const DoctorCase& Definition() const { return DoctorCases[Case]; }
    const Step& CurrentStep() const { return Definition().Steps[Stage < 3 ? Stage : 2]; }
    int Reward() const { return IsComplete() ? (Mistakes >= 4 ? 30 : 50 - Mistakes * 5) : 0; }
private:
    int Case = 0;
    int Stage = 0;
    int Mistakes = 0;
    bool Active = false;
    bool Complete = false;
};

inline bool CanCredit(std::int64_t Balance, std::int32_t Experience, int Reward)
{
    return Balance >= 0 && Experience >= 0 && Reward > 0 && Reward <= 50 &&
        Balance <= std::numeric_limits<std::int64_t>::max() - Reward &&
        Experience <= std::numeric_limits<std::int32_t>::max() - 25;
}
}
