#pragma once

#include <stdint.h>

class BoilerStateHandler
{
private:
    enum class BoilerState
    {
        BOILER_ABOVE_TARGET_AND_FILLED,
        BOILER_ABOVE_TARGET_BUT_FILLING,
        BOILER_BELOW_TARGET
    };

    bool *isFillingBoiler;

    uint32_t lastCheckTime = 0;

    bool hasTurnedOnBoilerProbeVoltage = false;

    BoilerState internalState = BoilerState::BOILER_ABOVE_TARGET_AND_FILLED;

    bool readBoilerProbe();

public:
    explicit BoilerStateHandler(bool *isFillingBoiler);

    void handleState();
};
