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

class StateActor
{
private:
    State *state;

    Relay pumpRelay;

    Solenoid boilerFillSolenoid;
    Solenoid groupOneSolenoid;
#ifdef MACHINE_HAS_DUAL_GROUP
    Solenoid groupTwoSolenoid;
#endif

    Solenoid teaWaterSolenoid;

public:
    explicit StateActor(State *state)
        : state(state), pumpRelay(RELAY_PUMP, "Pump", machineConfig.relayConfig.pumpInverted),
          boilerFillSolenoid(RELAY_BOILER_FILL, "Boiler Fill", machineConfig.relayConfig.boilerFillInverted),
          groupOneSolenoid(RELAY_GROUP_ONE, "Group One", machineConfig.relayConfig.groupOneInverted),
#ifdef MACHINE_HAS_DUAL_GROUP
          groupTwoSolenoid(RELAY_GROUP_TWO, "Group Two", machineConfig.relayConfig.groupTwoInverted),
#endif
          teaWaterSolenoid(RELAY_TEA, "Tea Water Cold Water", machineConfig.relayConfig.teaInverted) {};

    void loop()
    {
        // Boiler
        boilerFillSolenoid.setOpen(state->isFillingBoiler);

        // Group Heads
        groupOneSolenoid.setOpen(state->groupOneIsExtracting);

#ifdef MACHINE_HAS_DUAL_GROUP
        groupTwoSolenoid.setOpen(state->groupTwoIsExtracting);
#endif

        // Tea Water
        teaWaterSolenoid.setOpen(state->isExtractingTeaWater);

// Pump
#ifdef MACHINE_HAS_DUAL_GROUP
        // Dual Groups
        pumpRelay.setEnabled(state->isFillingBoiler || state->groupOneIsExtracting || state->groupTwoIsExtracting);
#else
        // Single Group
        pumpRelay.setEnabled(state->isFillingBoiler || state->groupOneIsExtracting);
#endif
    };
};

#endif // BREWPILOT_STATEACTOR_H
