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

    bool *isFillingBoiler;

    unsigned long lastCheckTime = 0;
    unsigned long lastIsAboveTargetTime = 0;

    BoilerState internalState = BOILER_ABOVE_TARGET_AND_FILLED;

    // TODO: Maybe Implement smoothing
    bool readBoilerProbe()
    {
        lastCheckTime = millis();

        uint16_t rawValue = analogRead(BOILER_PROBE_PIN);

        Serial.print("Raw Value: ");
        Serial.println(rawValue);
        Serial.print("Stored Value: ");
        Serial.println(BOILER_FILL_LEVEL);

        return rawValue <= BOILER_FILL_LEVEL;
    }

public:
    explicit BoilerStateHandler(bool *isFillingBoiler) : isFillingBoiler(isFillingBoiler)
    {
        pinMode(BOILER_PROBE_PIN, INPUT);
    };

    void handleState() override
    {
        // TODO: Add Debouncing here
        if (internalState == BOILER_ABOVE_TARGET_BUT_FILLING)
        {
            if (millis() - lastIsAboveTargetTime > 500)
            {
                bool isFilled = readBoilerProbe();

                if (!isFilled)
                {
                    Serial.println("Boiler is below target");
                    internalState = BOILER_BELOW_TARGET;
                }
                else
                {
                    Serial.println("Boiler is above target and filled");
                    internalState = BOILER_ABOVE_TARGET_AND_FILLED;
                    *isFillingBoiler = false;

                    lastIsAboveTargetTime = 0;
                }
            }
        }
        else
        {
            if (millis() - lastCheckTime > 500 || lastCheckTime == 0)
            {
                bool boilerIsFilled = readBoilerProbe();

                switch (internalState)
                {
                case BOILER_ABOVE_TARGET_AND_FILLED:
                    if (!boilerIsFilled)
                    {
                        Serial.println("Boiler is below target");
                        *isFillingBoiler = true;
                        internalState = BOILER_BELOW_TARGET;
                    }
                    break;
                case BOILER_ABOVE_TARGET_BUT_FILLING:
                    // Handler see above
                    break;
                case BOILER_BELOW_TARGET:
                    if (boilerIsFilled)
                    {
                        Serial.println("Boiler is above target and filled");
                        internalState = BOILER_ABOVE_TARGET_BUT_FILLING;
                    }
                    break;
                }
            }
        }
    }
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
