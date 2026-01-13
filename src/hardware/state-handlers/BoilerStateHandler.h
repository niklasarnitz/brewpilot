//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BOILERSTATEHANDLER_H
#define BREWPILOT_BOILERSTATEHANDLER_H

#include "GenericStateHandler.h"
#include "Arduino.h"
#include "../../StaticConfig.h"

#define BOILER_FILL_WAIT_TIME 2000

class BoilerStateHandler : public GenericStateHandler
{
private:
    enum class BoilerState
    {
        BOILER_ABOVE_TARGET_AND_FILLED,
        BOILER_ABOVE_TARGET_BUT_FILLING,
        BOILER_BELOW_TARGET
    };

    bool *isFillingBoiler;
    unsigned long lastCheckTime = 0;
    bool hasTurnedOnBoilerProbeVoltage = false;
    BoilerState internalState = BoilerState::BOILER_ABOVE_TARGET_AND_FILLED;
    bool readBoilerProbe();

public:
    explicit BoilerStateHandler(bool *isFillingBoiler);
    void handleState() override;
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
