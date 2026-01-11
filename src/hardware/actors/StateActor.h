//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_STATEACTOR_H
#define BREWPILOT_STATEACTOR_H

#include "hardware/state/State.h"
#include "hardware/devices/Relay.h"
#include "hardware/devices/Solenoid.h"
#include "StaticConfig.h"
#include "configs/MachineConfig.h"
#include "configs/getMachineConfig.h"

const MachineConfig config = getMachineConfig();

class StateActor
{
private:
    State *state;

    Relay pumpRelay;

    Solenoid boilerFillSolenoid;
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterSolenoid;

public:
    explicit StateActor(State *state);

    void loop();
};

#endif // BREWPILOT_STATEACTOR_H
