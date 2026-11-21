#pragma once

#include "config/SelectedMachine.h"
#include "drivers/Relay.h"
#include "drivers/Solenoid.h"
#include "state/State.h"

class StateActor
{
private:
    const State &state;

    Relay pumpRelay;

    Solenoid boilerFillSolenoid;
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterSolenoid;

public:
    explicit StateActor(const State &state);

    void loop();
};
