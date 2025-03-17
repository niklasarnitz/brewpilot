//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BOILERSTATEHANDLER_H
#define BREWPILOT_BOILERSTATEHANDLER_H

#include "GenericStateHandler.h"
#include "Arduino.h"
#include "../../Config.h"

class BoilerStateHandler : public GenericStateHandler
{
private:
    enum BoilerState
    {
        BOILER_ABOVE_TARGET_AND_FILLED,
        BOILER_ABOVE_TARGET_BUT_FILLING,
        BOILER_BELOW_TARGET
    };

    bool &isFillingBoiler;

    unsigned long lastCheckTime = 0;
    unsigned long lastIsAboveTargetTime = 0;

    BoilerState internalState = BOILER_ABOVE_TARGET_AND_FILLED;

    // TODO: Maybe Implement smoothing
    bool readBoilerProbe()
    {
        lastCheckTime = millis();

        return analogRead(BOILER_PROBE_PIN) <= BOILER_FILL_LEVEL;
    }

public:
    explicit BoilerStateHandler(bool &isFillingBoiler) : isFillingBoiler(isFillingBoiler)
    {
        pinMode(BOILER_PROBE_PIN, INPUT);
    };

    void handleState() override
    {
        if (internalState == BOILER_ABOVE_TARGET_BUT_FILLING)
        {
            if (millis() - lastIsAboveTargetTime > 2000)
            {
                bool isFilled = readBoilerProbe();

                if (!isFilled)
                {
                    internalState = BOILER_BELOW_TARGET;
                }
                else
                {
                    internalState = BOILER_ABOVE_TARGET_AND_FILLED;
                    isFillingBoiler = false;

                    lastIsAboveTargetTime = 0;
                }
            }
        }
        else
        {
            if (millis() - lastCheckTime > 2000 || lastCheckTime == 0)
            {
                bool boilerIsFilled = readBoilerProbe();

                switch (internalState)
                {
                case BOILER_ABOVE_TARGET_AND_FILLED:
                    if (!boilerIsFilled)
                    {
                        isFillingBoiler = true;
                        internalState = BOILER_BELOW_TARGET;
                    }
                    break;
                case BOILER_ABOVE_TARGET_BUT_FILLING:
                    // Handler see above
                    break;
                case BOILER_BELOW_TARGET:
                    if (boilerIsFilled)
                    {
                        internalState = BOILER_ABOVE_TARGET_BUT_FILLING;
                    }
                    break;
                }
            }
        }
    }
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
