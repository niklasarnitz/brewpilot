#pragma once

#include "config/BuildConfig.h"
#include "drivers/Relay.h"
#include "drivers/Solenoid.h"
#include "state/State.h"

class StateActor
{
private:
    const State &state;

    Relay pumpRelay;

    Solenoid boilerFillSolenoid;
#if BREWPILOT_GROUP_ONE_ACTIVE
    Solenoid groupOneSolenoid;
#endif
#if BREWPILOT_GROUP_TWO_ACTIVE
    Solenoid groupTwoSolenoid;
#endif
    Solenoid teaWaterSolenoid;

public:
    explicit StateActor(const State &state);

    void loop();
};
