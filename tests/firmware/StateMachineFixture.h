#pragma once

#include <Arduino.h>
#include <iostream>
#include <stdexcept>
#include "config/BoardPins.h"
#include "config/SelectedMachine.h"
#include "output/StateActor.h"
#include "state/StateHandler.h"

using Event = GroupHeadButtonEvent;

// Group numbers this machine has; whole-machine scenarios loop over these.
inline constexpr int activeGroups[] = {
#if BREWPILOT_GROUP_ONE_ACTIVE
    1,
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    2,
#endif
};

inline void require(bool condition, const char *message)
{
    if (!condition)
        throw std::runtime_error(message);
}

// Only the hardware boundary is simulated; handlers and persistence are real.
struct StateMachineFixture
{
    PreferenceHelper preferences;
    VolumetricsHelper volumes{preferences};
    State state{};
    ButtonEvent events{};
    StateHandler handler{state, events, volumes};
    StateActor actor{state};

    StateMachineFixture()
    {
        testStorage.clear();
        testFailOpen = testFailWrite = false;
        testMillis = 1;
        testProbe = 0;
    }

    void tick(Event one = Event::NONE, Event two = Event::NONE, bool tea = false, uint32_t elapsed = 50, bool teaHeld = false)
    {
        testMillis += elapsed;
        events = {one, teaHeld ? ButtonEventType::BUTTON_HELD :
                           (tea ? ButtonEventType::BUTTON_PRESSED : ButtonEventType::BUTTON_NONE), two};
        handler.handleState();
        actor.loop();
    }

    void pulses(int group, unsigned count)
    {
        while (count--)
        {
#if BREWPILOT_GROUP_ONE_ACTIVE
            if (group == 1)
            {
                handler.groupOneFlowMeterPulseInterrupt();
                continue;
            }
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
            if (group == 2)
            {
                handler.groupTwoFlowMeterPulseInterrupt();
                continue;
            }
#endif
            throw std::logic_error("this machine has no flow meter for that group");
        }
    }

    void selectProgramming(int group = 1)
    {
        tick(group == 1 ? Event::CONTINUOUS : Event::NONE,
             group == 2 ? Event::CONTINUOUS : Event::NONE);
        tick(group == 1 ? Event::CONTINUOUS_HELD : Event::NONE,
             group == 2 ? Event::CONTINUOUS_HELD : Event::NONE, false, 2000);
        require(state.programmingTarget == (group == 1 ? ProgrammingTarget::GROUP_ONE : ProgrammingTarget::GROUP_TWO),
                "holding continuous must reserve that group's programming session");
    }
};
