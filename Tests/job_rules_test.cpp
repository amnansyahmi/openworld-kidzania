#include "Domain/JobRules.h"
#include <cassert>
#include <iostream>
using namespace KidzCity::Rules;
int main()
{
    DoctorSession session;
    assert(session.Answer(0) == AnswerResult::Inactive);
    assert(!session.Start(-1));
    assert(!session.Start(2));
    for (int c = 0; c < 2; ++c)
    {
        assert(session.Start(c));
        assert(!session.Start(c));
        assert(session.Reward() == 0);
        assert(session.Answer(-1) == AnswerResult::Inactive);
        assert(session.StepIndex() == 0);
        const int wrong = (session.CurrentStep().Correct + 1) % 3;
        assert(session.Answer(wrong) == AnswerResult::Wrong);
        assert(session.StepIndex() == 0);
        for (int step = 0; step < 3; ++step)
        {
            const auto result = session.Answer(session.CurrentStep().Correct);
            assert(result == (step == 2 ? AnswerResult::Completed : AnswerResult::Advanced));
        }
        assert(session.IsComplete());
        assert(session.Reward() == 45);
        assert(session.Answer(0) == AnswerResult::Inactive);
        session.Cancel();
        assert(session.Reward() == 0);
    }
    session.Start(0);
    for (int i = 0; i < 1000; ++i) session.Answer(0);
    while (!session.IsComplete()) session.Answer(session.CurrentStep().Correct);
    assert(session.Reward() == 30);
    assert(CanCredit(0, 0, 50));
    assert(!CanCredit(-1, 0, 50));
    assert(!CanCredit(0, -1, 50));
    assert(!CanCredit(0, 0, 51));
    assert(!CanCredit(INT64_MAX, 0, 50));
    assert(!CanCredit(0, INT32_MAX, 50));
    std::cout << "Job rules: all assertions passed\n";
}
