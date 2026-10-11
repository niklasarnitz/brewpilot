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
    BoilerStateHandler boilerStateHandler;
    GroupHeadStateHandler groupOneStateHandler;
    GroupHeadStateHandler groupTwoStateHandler;
    TeaStateHandler teaStateHandler;

public:
    StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper)
        : state(state), buttonEvent(buttonEvent),
          boilerStateHandler(&state->isFillingBoiler),
          groupOneStateHandler(&state->groupOneIsExtracting,
                               &buttonEvent->groupOne, volumetricsHelper, &state->programmingTarget, 1),
          groupTwoStateHandler(&state->groupTwoIsExtracting,
                               &buttonEvent->groupTwo, volumetricsHelper, &state->programmingTarget, 2),
          teaStateHandler(&buttonEvent->tea,
                          &state->isExtractingTeaWater, volumetricsHelper, &state->programmingTarget) {};

    void handleState() override
    {
        boilerStateHandler.handleState();
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
