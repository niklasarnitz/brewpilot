#include "state/StateHandler.h"

#include <Arduino.h>

StateHandler::StateHandler(State &state, const ButtonEvent &buttonEvent, VolumetricsHelper &volumetricsHelper)
    : boilerStateHandler(state.isFillingBoiler),
#if BREWPILOT_GROUP_ONE_ACTIVE
      groupOneStateHandler(state.groupOneIsExtracting,
                           buttonEvent.groupOne, volumetricsHelper, state.programmingTarget, 1),
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
      groupTwoStateHandler(state.groupTwoIsExtracting,
                           buttonEvent.groupTwo, volumetricsHelper, state.programmingTarget, 2),
#endif
      teaStateHandler(buttonEvent.tea,
                      state.isExtractingTeaWater, volumetricsHelper, state.programmingTarget) {}

void StateHandler::handleState()
{
    boilerStateHandler.handleState();
#if BREWPILOT_GROUP_ONE_ACTIVE
    groupOneStateHandler.handleState();
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    groupTwoStateHandler.handleState();
#endif
    teaStateHandler.handleState();
}

#if BREWPILOT_GROUP_ONE_ACTIVE
void IRAM_ATTR StateHandler::groupOneFlowMeterPulseInterrupt()
{
    groupOneStateHandler.flowMeterPulseInterrupt();
}
#endif

#if BREWPILOT_GROUP_TWO_ACTIVE
void IRAM_ATTR StateHandler::groupTwoFlowMeterPulseInterrupt()
{
    groupTwoStateHandler.flowMeterPulseInterrupt();
}
#endif
