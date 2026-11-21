#pragma once

#include "input/ButtonEvents.h"
#include "state/BoilerStateHandler.h"
#include "state/GenericStateHandler.h"
#include "state/GroupHeadStateHandler.h"
#include "state/State.h"
#include "state/TeaStateHandler.h"
#include "storage/VolumetricsHelper.h"

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
    StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper);

    void handleState() override;

    // Run in interrupt context and are placed in IRAM.
    void groupOneFlowMeterPulseInterrupt();
    void groupTwoFlowMeterPulseInterrupt();
};
