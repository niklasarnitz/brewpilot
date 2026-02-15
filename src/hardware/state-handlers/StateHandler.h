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
    BoilerStateHandler boilerStateHandler;
    GroupHeadStateHandler groupOneStateHandler;
    GroupHeadStateHandler groupTwoStateHandler;
    TeaStateHandler teaStateHandler;

public:
    StateHandler(State *state, ButtonEvent *buttonEvent, VolumetricsHelper *volumetricsHelper, PreferenceHelper *preferenceHelper);
    void handleState() override;
    void groupOneFlowMeterPulseInterrupt();
    void groupTwoFlowMeterPulseInterrupt();

    // Reload auto-backflush settings for both groups
    void reloadAutoBackflushSettings();

    // Reload backflush settings for both groups
    void reloadBackflushSettings();

    // BLE access to group head progress
    const GroupHeadStateHandler &getGroupOneHandler() const { return groupOneStateHandler; }
    const GroupHeadStateHandler &getGroupTwoHandler() const { return groupTwoStateHandler; }
    const BoilerStateHandler &getBoilerHandler() const { return boilerStateHandler; }
};

#endif // BREWPILOT_STATEHANDLER_H
