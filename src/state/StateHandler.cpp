#include "state/StateHandler.h"

#include <Arduino.h>

StateHandler::StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper)
    : state(state), buttonEvent(buttonEvent),
      boilerStateHandler(&state->isFillingBoiler),
      groupOneStateHandler(&state->groupOneIsExtracting,
                           &buttonEvent->groupOne, volumetricsHelper, &state->programmingTarget, 1),
      groupTwoStateHandler(&state->groupTwoIsExtracting,
                           &buttonEvent->groupTwo, volumetricsHelper, &state->programmingTarget, 2),
      teaStateHandler(&buttonEvent->tea,
                      &state->isExtractingTeaWater, volumetricsHelper, &state->programmingTarget) {}

void StateHandler::handleState()
{
    boilerStateHandler.handleState();
    groupOneStateHandler.handleState();
    groupTwoStateHandler.handleState();
    teaStateHandler.handleState();
}

void IRAM_ATTR StateHandler::groupOneFlowMeterPulseInterrupt()
{
    groupOneStateHandler.flowMeterPulseInterrupt();
}

void IRAM_ATTR StateHandler::groupTwoFlowMeterPulseInterrupt()
{
    groupTwoStateHandler.flowMeterPulseInterrupt();
}
