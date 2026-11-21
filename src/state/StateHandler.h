#pragma once

#include "config/BuildConfig.h"
#include "input/ButtonEvents.h"
#include "state/BoilerStateHandler.h"
#include "state/GroupHeadStateHandler.h"
#include "state/State.h"
#include "state/TeaStateHandler.h"
#include "storage/VolumetricsHelper.h"

class StateHandler
{
private:
    BoilerStateHandler boilerStateHandler;
#if BREWPILOT_GROUP_ONE_ACTIVE
    GroupHeadStateHandler groupOneStateHandler;
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    GroupHeadStateHandler groupTwoStateHandler;
#endif
    TeaStateHandler teaStateHandler;

public:
    StateHandler(State &state, const ButtonEvent &buttonEvent, VolumetricsHelper &volumetricsHelper);

    void handleState();

    // Run in interrupt context and are placed in IRAM.
#if BREWPILOT_GROUP_ONE_ACTIVE
    void groupOneFlowMeterPulseInterrupt();
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    void groupTwoFlowMeterPulseInterrupt();
#endif
};
