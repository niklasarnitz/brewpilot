#pragma once

#include "drivers/Relay.h"
#include "drivers/Solenoid.h"
#include "state/State.h"

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
