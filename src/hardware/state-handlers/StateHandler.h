//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_STATEHANDLER_H
#define BREWPILOT_STATEHANDLER_H

#include "../state/State.h"
#include "../../structs/ButtonEvent.h"
#include "BoilerStateHandler.h"
#include "GroupHeadHandler.h"
#include "TeaStateHandler.h"
#include "../../utils/VolumetricsHelper.h"

class StateHandler : public GenericStateHandler
{
private:
    State *state;
    ButtonEvent *buttonEvent;
    bool groupOneAckArmed = false;
    bool groupTwoAckArmed = false;
    BoilerStateHandler boilerStateHandler;
    GroupHeadStateHandler groupOneStateHandler;
    GroupHeadStateHandler groupTwoStateHandler;
    TeaStateHandler teaStateHandler;

public:
    StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper)
        : state(state), buttonEvent(buttonEvent),
          boilerStateHandler(&state->isFillingBoiler, &state->boilerFillFault),
          groupOneStateHandler(&state->groupOneIsExtracting,
                               &buttonEvent->groupOne, volumetricsHelper, &state->programmingTarget, 1),
          groupTwoStateHandler(&state->groupTwoIsExtracting,
                               &buttonEvent->groupTwo, volumetricsHelper, &state->programmingTarget, 2),
          teaStateHandler(&buttonEvent->tea,
                          &state->isExtractingTeaWater, volumetricsHelper, &state->programmingTarget) {};

    void handleState() override
    {
        const bool wasFaulted = state->boilerFillFault;
        boilerStateHandler.handleState();
        if (state->boilerFillFault)
        {
            groupOneStateHandler.stop();
            groupTwoStateHandler.stop();
            state->isExtractingTeaWater = false;
            state->programmingTarget = ProgrammingTarget::NONE;

            if (!wasFaulted)
            {
                groupOneAckArmed = groupTwoAckArmed = false;
            }
            else
            {
                // A button already held when the fault arose must not acknowledge it.
                if (buttonEvent->groupOne == GroupHeadButtonEvent::CONTINUOUS)
                    groupOneAckArmed = true;
                if (buttonEvent->groupTwo == GroupHeadButtonEvent::CONTINUOUS)
                    groupTwoAckArmed = true;
                if ((groupOneAckArmed && buttonEvent->groupOne == GroupHeadButtonEvent::CONTINUOUS_HELD) ||
                    (groupTwoAckArmed && buttonEvent->groupTwo == GroupHeadButtonEvent::CONTINUOUS_HELD))
                {
                    boilerStateHandler.acknowledgeFault();
                    groupOneAckArmed = groupTwoAckArmed = false;
                }
            }
            return; // Consume acknowledgement; never use it to start a drink or programming.
        }
        groupOneStateHandler.handleState();
        groupTwoStateHandler.handleState();
        teaStateHandler.handleState();
    }

    void groupOneFlowMeterPulseInterrupt()
    {
        groupOneStateHandler.flowMeterPulseInterrupt();
    }

    void groupTwoFlowMeterPulseInterrupt()
    {
        groupTwoStateHandler.flowMeterPulseInterrupt();
    }
};

#endif // BREWPILOT_STATEHANDLER_H
