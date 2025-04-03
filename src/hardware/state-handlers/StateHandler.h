//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_STATEHANDLER_H
#define BREWPILOT_STATEHANDLER_H

#include "../state/State.h"
#include "BoilerStateHandler.h"
#include "GroupHeadHandler.h"
#include "TeaStateHandler.h"

class StateHandler : public GenericStateHandler
{
private:
    BoilerStateHandler boilerStateHandler;
    GroupHeadStateHandler groupOneStateHandler;
    GroupHeadStateHandler groupTwoStateHandler;
    TeaStateHandler teaStateHandler;

public:
    StateHandler(State *state, ButtonEvent *buttonEvent) : boilerStateHandler(&state->isFillingBoiler),
                                                           groupOneStateHandler(&state->groupOneIsExtracting,
                                                                                &buttonEvent->groupOne),
                                                           groupTwoStateHandler(&state->groupTwoIsExtracting,
                                                                                &buttonEvent->groupTwo),
                                                           teaStateHandler(&buttonEvent->tea,
                                                                           &state->isExtractingTeaWater) {};

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
