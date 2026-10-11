#pragma once

#include <iostream>
#include <stdexcept>
#include "hardware/state-handlers/StateHandler.h"
#include "hardware/actors/StateActor.h"

using Event = GroupHeadButtonEvent;

inline void require(bool condition, const char *message)
{
    if (!condition)
        throw std::runtime_error(message);
}

// Only the hardware boundary is simulated; handlers and persistence are real.
struct StateMachineFixture
{
    PreferenceHelper preferences;
    VolumetricsHelper volumes{&preferences};
    State state{};
    ButtonEvent events{};
    StateHandler handler{&state, &events, &volumes};
    StateActor actor{&state};

    StateMachineFixture()
    {
        testStorage.clear();
        testFailOpen = testFailWrite = false;
        testMillis = 1;
        testProbe = 0;
    }

    void tick(Event one = Event::NONE, Event two = Event::NONE, bool tea = false, uint32_t elapsed = 50)
    {
        testMillis += elapsed;
        events = {one, tea, two};
        handler.handleState();
        actor.loop();
    }

    void pulses(int group, unsigned count)
    {
        while (count--)
        {
            if (group == 1)
                handler.groupOneFlowMeterPulseInterrupt();
            else
                handler.groupTwoFlowMeterPulseInterrupt();
        }
    }

    void selectProgramming(int group = 1)
    {
        tick(group == 1 ? Event::CONTINUOUS : Event::NONE,
             group == 2 ? Event::CONTINUOUS : Event::NONE);
        tick(group == 1 ? Event::CONTINUOUS_HELD : Event::NONE,
             group == 2 ? Event::CONTINUOUS_HELD : Event::NONE, false, 2000);
        require(state.programmingTarget == ProgrammingTarget::SELECTING,
                "holding continuous must enter programming selection");
    }
};
