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

    Relay pumpRelay{RELAY_PUMP, "Pump", config.relayConfig.pumpInverted};

    Solenoid boilerFillSolenoid{RELAY_BOILER_FILL, "Boiler Fill", config.relayConfig.boilerFillInverted};
    Solenoid groupOneSolenoid{RELAY_GROUP_ONE, "Group One", config.relayConfig.groupOneInverted};
    Solenoid groupTwoSolenoid{RELAY_GROUP_TWO, "Group Two", config.relayConfig.groupTwoInverted};
    Solenoid teaWaterSolenoid{RELAY_TEA, "Tea Water Cold Water", config.relayConfig.teaInverted};

public:
    explicit StateActor(State *state);

    void loop();
};

#endif // BREWPILOT_STATEACTOR_H
