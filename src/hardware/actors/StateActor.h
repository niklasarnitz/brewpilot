//
// Created by Niklas Arnitz on 17.11.24.
//

#ifndef BREWPILOT_STATEACTOR_H
#define BREWPILOT_STATEACTOR_H

#include "hardware/state/State.h"
#include "hardware/devices/Relay.h"
#include "hardware/devices/Solenoid.h"
#include "Config.h"

class StateActor {
private:
    Solenoid boilerFillSolenoid;
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterColdWaterSolenoid;
    Solenoid teaWaterExtractionSolenoid;

    Relay pumpRelay;

    State &state;

public:
    explicit StateActor(State &state) : state(state), pumpRelay(PUMP_RELAY_PIN, false),
                                        boilerFillSolenoid(BOILER_FILL_VALVE_PIN, false),
                                        groupOneSolenoid(GROUP_ONE_SOLENOID_PIN, false),
                                        groupTwoSolenoid(GROUP_TWO_SOLENOID_PIN, false),
                                        teaWaterColdWaterSolenoid(TEA_WATER_SOLENOID_PIN, false),
                                        teaWaterExtractionSolenoid(TEA_WATER_EXTRACTION_SOLENOID_PIN, false) {}

    void loop() {
        // Boiler
        boilerFillSolenoid.setOpen(state.isFillingBoiler);

        // Group Heads
        groupOneSolenoid.setOpen(state.groupOneIsExtracting);
        groupTwoSolenoid.setOpen(state.groupTwoIsExtracting);

        // Tea Water
        teaWaterColdWaterSolenoid.setOpen(state.isExtractingTeaWater);
        teaWaterExtractionSolenoid.setOpen(state.isExtractingTeaWater);

        // Pump
        pumpRelay.setEnabled(state.isFillingBoiler || state.groupOneIsExtracting || state.groupTwoIsExtracting);
    };
};

#endif //BREWPILOT_STATEACTOR_H
