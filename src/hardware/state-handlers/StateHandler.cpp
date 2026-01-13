//
// Created by Niklas Arnitz on 16.11.24.
//

#include "StateHandler.h"

StateHandler::StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper)
    : boilerStateHandler(&state->isFillingBoiler),
      groupOneStateHandler(&state->groupOneIsExtracting, &buttonEvent->groupOne, volumetricsHelper, &state->isInProgrammingMode, 1),
      groupTwoStateHandler(&state->groupTwoIsExtracting, &buttonEvent->groupTwo, volumetricsHelper, &state->isInProgrammingMode, 2),
      teaStateHandler(&buttonEvent->tea, &state->isExtractingTeaWater, volumetricsHelper, &state->isInProgrammingMode)
{
}

void StateHandler::handleState()
{
    boilerStateHandler.handleState();
    groupOneStateHandler.handleState();
    groupTwoStateHandler.handleState();
    teaStateHandler.handleState();
}

void StateHandler::groupOneFlowMeterPulseInterrupt()
{
    groupOneStateHandler.flowMeterPulseInterrupt();
}

void StateHandler::groupTwoFlowMeterPulseInterrupt()
{
    groupTwoStateHandler.flowMeterPulseInterrupt();
}
