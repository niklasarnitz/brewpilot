//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_STATEACTOR_H
#define BREWPILOT_STATEACTOR_H

#include "hardware/state/State.h"
#include "hardware/devices/Relay.h"
#include "hardware/devices/Solenoid.h"
#include "StaticConfig.h"

class StateActor
{
private:
    Solenoid boilerFillSolenoid;
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterSolenoid;

    Relay pumpRelay;

    State *state;

public:
    explicit StateActor(State *state) : state(state), pumpRelay(RELAY_PUMP, "Pump"),
                                        boilerFillSolenoid(RELAY_BOILER_FILL, "Boiler Fill"),
                                        groupOneSolenoid(RELAY_GROUP_ONE, "Group One"),
                                        groupTwoSolenoid(RELAY_GROUP_TWO, "Group Two"),
                                        teaWaterSolenoid(RELAY_TEA, "Tea Water Cold Water") {}

    void loop()
    {
        // Boiler
        boilerFillSolenoid.setOpen(state->isFillingBoiler);

        // Group Heads
        groupOneSolenoid.setOpen(state->groupOneIsExtracting);
        groupTwoSolenoid.setOpen(state->groupTwoIsExtracting);

        // Tea Water
        teaWaterSolenoid.setOpen(state->isExtractingTeaWater);

        // Pump
        pumpRelay.setEnabled(state->isFillingBoiler || state->groupOneIsExtracting || state->groupTwoIsExtracting);
    };
};

#endif // BREWPILOT_STATEACTOR_H
