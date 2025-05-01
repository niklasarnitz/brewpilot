//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BOILERSTATEHANDLER_H
#define BREWPILOT_BOILERSTATEHANDLER_H

#include "GenericStateHandler.h"
#include "Arduino.h"
#include "../../StaticConfig.h"

#define BOILER_FILL_WAIT_TIME 3000

extern void logMessage(const char *message);

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
    unsigned long lastIsAboveTargetTime = 0;

    bool hasTurnedOnBoilerProbeVoltage = false;

    BoilerState internalState = BoilerState::BOILER_ABOVE_TARGET_AND_FILLED;

    // TODO: Maybe Implement smoothing
    bool readBoilerProbe()
    {
        if (!hasTurnedOnBoilerProbeVoltage)
        {
            digitalWrite(OUT_BOILER_VOLTAGE, HIGH);
            hasTurnedOnBoilerProbeVoltage = true;

            return internalState != BoilerState::BOILER_BELOW_TARGET;
        }
        else
        {
            lastCheckTime = millis();

            uint16_t rawValue = analogRead(BOILER_PROBE_PIN);

            char buffer[64];
            sprintf(buffer, "Boiler Fill Check: Raw Sensor Value: %d", rawValue);
            logMessage(buffer);

            digitalWrite(OUT_BOILER_VOLTAGE, LOW);
            hasTurnedOnBoilerProbeVoltage = false;

            return rawValue <= BOILER_FILL_LEVEL;
        }
    }

public:
    explicit BoilerStateHandler(bool *isFillingBoiler)
        : isFillingBoiler(isFillingBoiler)
    {
        pinMode(BOILER_PROBE_PIN, INPUT);
        pinMode(OUT_BOILER_VOLTAGE, OUTPUT);

        digitalWrite(OUT_BOILER_VOLTAGE, LOW);
    };

    void handleState() override
    {
        if (internalState == BoilerState::BOILER_ABOVE_TARGET_BUT_FILLING)
        {
            if (millis() - lastIsAboveTargetTime > BOILER_FILL_WAIT_TIME)
            {
                bool isFilled = readBoilerProbe();

                if (!isFilled)
                {
                    logMessage("Boiler Fill Check: Boiler is below target");
                    internalState = BoilerState::BOILER_BELOW_TARGET;
                }
                else
                {
                    logMessage("Boiler Fill Check: Boiler is above target and filled");
                    internalState = BoilerState::BOILER_ABOVE_TARGET_AND_FILLED;
                    *isFillingBoiler = false;

                    lastIsAboveTargetTime = 0;
                }
            }
        }
        else
        {
            if (millis() - lastCheckTime > BOILER_FILL_WAIT_TIME || lastCheckTime == 0)
            {
                bool boilerIsFilled = readBoilerProbe();

                switch (internalState)
                {
                case BoilerState::BOILER_ABOVE_TARGET_AND_FILLED:
                    if (!boilerIsFilled)
                    {
                        logMessage("Boiler Fill Check: Boiler is below target");
                        *isFillingBoiler = true;
                        internalState = BoilerState::BOILER_BELOW_TARGET;
                    }
                    break;
                case BoilerState::BOILER_ABOVE_TARGET_BUT_FILLING:
                    // Handler see above
                    break;
                case BoilerState::BOILER_BELOW_TARGET:
                    if (boilerIsFilled)
                    {
                        logMessage("Boiler Fill Check: Boiler is above target and filled");
                        internalState = BoilerState::BOILER_ABOVE_TARGET_BUT_FILLING;
                    }
                    break;
                }
            }
        }
    }
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
