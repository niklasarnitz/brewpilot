#include "StateMachineFixture.h"
#include "hardware/handlers/ButtonHandler.h"
#include "hardware/handlers/GroupHeadButtonHandler.h"

namespace
{
void buttonLifecycle()
{
    bool down = false;
    ButtonHandler button(&down);
    auto sample = [&](ButtonEventType expected, bool state) {
        const ButtonState actual = button.getEvent();
        require(actual.event == expected, "button event must match the independent sample expectation");
        require(actual.currentState == state, "button currentState must follow the input");
    };
    sample(ButtonEventType::BUTTON_NONE, false);
    // Short releases and long holds must both reset the entire streak.
    for (int duration : {7, 213, 41, 89})
    {
        down = true;
        for (int index = 1; index <= duration; ++index)
            sample(index == 1 ? ButtonEventType::BUTTON_PRESSED
                             : index == 40 ? ButtonEventType::BUTTON_HELD
                                           : ButtonEventType::BUTTON_NONE,
                   true);
        down = false;
        sample(ButtonEventType::BUTTON_NONE, false);
        sample(ButtonEventType::BUTTON_NONE, false);
    }
}

struct ButtonFixture
{
    GroupButtonMatrixState keys{};
    Event event = Event::RIGHT_DOUBLE_ESPRESSO;
    GroupHeadButtonHandler handler;

    explicit ButtonFixture(int group) : handler(&keys, &event, group) {}

    void sample(Event expected)
    {
        handler.handle();
        require(event == expected, "group button event must match the independent expectation");
    }

    void quiet(int count)
    {
        while (count--)
            sample(Event::NONE);
    }
};

struct DoseCase
{
    bool GroupButtonMatrixState::*key;
    Event expected;
};

constexpr DoseCase doseCases[] = {
    {&GroupButtonMatrixState::leftSingleDose, Event::LEFT_SINGLE_ESPRESSO},
    {&GroupButtonMatrixState::leftDoubleDose, Event::LEFT_DOUBLE_ESPRESSO},
    {&GroupButtonMatrixState::rightSingleDose, Event::RIGHT_SINGLE_ESPRESSO},
    {&GroupButtonMatrixState::rightDoubleDose, Event::RIGHT_DOUBLE_ESPRESSO}};

void dosesAndPriorities(int group)
{
    for (const auto &dose : doseCases)
    {
        ButtonFixture buttons(group);
        buttons.sample(Event::NONE);
        buttons.keys.*dose.key = true;
        buttons.sample(dose.expected);
        buttons.quiet(137); // Includes dose HELD and saturation: neither repeats a dose.
        buttons.keys.*dose.key = false;
        buttons.sample(Event::NONE);
        buttons.keys.*dose.key = true;
        buttons.sample(dose.expected);
    }

    // Every pair has an explicit priority, including the special backflush pair.
    constexpr Event winners[] = {Event::LEFT_SINGLE_ESPRESSO, Event::LEFT_SINGLE_ESPRESSO,
                                 Event::AUTO_BACKFLUSH, Event::LEFT_DOUBLE_ESPRESSO,
                                 Event::LEFT_DOUBLE_ESPRESSO, Event::RIGHT_SINGLE_ESPRESSO};
    int pair = 0;
    for (int first = 0; first < 4; ++first)
        for (int second = first + 1; second < 4; ++second)
        {
            ButtonFixture buttons(group);
            buttons.keys.*doseCases[first].key = true;
            buttons.keys.*doseCases[second].key = true;
            buttons.sample(winners[pair++]);
            buttons.quiet(67);
        }

    ButtonFixture buttons(group);
    buttons.keys.rightDoubleDose = true;
    buttons.sample(Event::RIGHT_DOUBLE_ESPRESSO);
    buttons.keys.leftDoubleDose = buttons.keys.rightSingleDose = true;
    buttons.sample(Event::LEFT_DOUBLE_ESPRESSO); // Held high/low priority keys are not new presses.
    buttons.quiet(53);
    buttons.keys = {};
    buttons.sample(Event::NONE);
    buttons.keys.leftSingleDose = true;
    buttons.sample(Event::LEFT_SINGLE_ESPRESSO);
    buttons.quiet(3);
    buttons.keys.rightSingleDose = true;
    buttons.sample(Event::RIGHT_SINGLE_ESPRESSO); // A held higher-priority key cannot swallow a fresh lower-priority press.

    ButtonFixture chord(group);
    chord.keys = {true, true, false, true, true};
    chord.sample(Event::AUTO_BACKFLUSH); // The chord outranks other fresh dose presses too.
}

void backflushCombinations(int group)
{
    for (bool leftFirst : {true, false})
    {
        ButtonFixture buttons(group);
        bool &first = leftFirst ? buttons.keys.leftSingleDose : buttons.keys.rightDoubleDose;
        bool &second = leftFirst ? buttons.keys.rightDoubleDose : buttons.keys.leftSingleDose;
        first = true;
        buttons.sample(leftFirst ? Event::LEFT_SINGLE_ESPRESSO : Event::RIGHT_DOUBLE_ESPRESSO);
        buttons.quiet(leftFirst ? 5 : 73);
        second = true;
        buttons.sample(Event::AUTO_BACKFLUSH);
        buttons.quiet(121);
        second = false;
        buttons.sample(Event::NONE);
        second = true;
        buttons.sample(Event::AUTO_BACKFLUSH);
        first = false;
        buttons.sample(Event::NONE);
        first = true;
        buttons.sample(Event::AUTO_BACKFLUSH);
        buttons.keys = {};
        buttons.sample(Event::NONE);
        buttons.keys.leftSingleDose = buttons.keys.rightDoubleDose = true;
        buttons.sample(Event::AUTO_BACKFLUSH);
        buttons.quiet(83);
    }
}

void stopPriority(int group)
{
    StateMachineFixture machine;
    require(machine.volumes.writeFlowMeterSetting(Event::RIGHT_SINGLE_ESPRESSO, 19),
            "post-Stop dose must have a nonzero configured target");
    ButtonFixture buttons(group);
    auto step = [&](Event expected) {
        buttons.sample(expected);
        machine.tick(group == 1 ? buttons.event : Event::NONE,
                     group == 2 ? buttons.event : Event::NONE);
    };
    auto stopped = [&] {
        require(!machine.state.groupOneIsExtracting && !machine.state.groupTwoIsExtracting &&
                    !machine.state.isExtractingTeaWater,
                "dose/combo inputs must not start water while Stop is held");
    };

    // Start backflush first, then cancel it with STOP competing against every dose.
    step(Event::NONE);
    buttons.keys.leftSingleDose = buttons.keys.rightDoubleDose = true;
    step(Event::AUTO_BACKFLUSH);
    require(group == 1 ? machine.state.groupOneIsExtracting : machine.state.groupTwoIsExtracting,
            "backflush must start before the Stop cancellation test");
    buttons.keys = {};
    step(Event::NONE);
    buttons.keys = {true, true, true, true, true};
    step(Event::CONTINUOUS);
    stopped();
    for (int sample = 2; sample <= 181; ++sample)
    {
        // Fresh asymmetric dose presses and a fresh combo coincide with HELD at 40.
        buttons.keys.leftSingleDose = buttons.keys.rightDoubleDose = sample == 40 || sample % 7 == 0;
        buttons.keys.leftDoubleDose = sample % 3 == 0;
        buttons.keys.rightSingleDose = sample % 11 == 0;
        step(sample == 40 ? Event::CONTINUOUS_HELD : Event::NONE);
        stopped();
    }
    buttons.keys = {};
    step(Event::NONE);
    buttons.keys.rightSingleDose = true;
    step(Event::RIGHT_SINGLE_ESPRESSO);
    require(group == 1 ? machine.state.groupOneIsExtracting : machine.state.groupTwoIsExtracting,
            "a fresh dose after Stop release must start water again");
    buttons.keys.continuous = true;
    step(Event::CONTINUOUS);
    stopped();
    buttons.keys = {};
    step(Event::NONE);
    buttons.keys.continuous = true;
    buttons.sample(Event::CONTINUOUS);
    buttons.quiet(38);
    buttons.sample(Event::CONTINUOUS_HELD);
    buttons.quiet(97);
}
} // namespace

void runButtonEventTests()
{
    buttonLifecycle();
    for (int group : {1, 2})
    {
        dosesAndPriorities(group);
        backflushCombinations(group);
        stopPriority(group);
    }
}
