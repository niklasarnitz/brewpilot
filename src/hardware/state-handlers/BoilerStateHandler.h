//
// Created by Niklas Arnitz on 16.11.24.
//

#ifndef BREWPILOT_BOILERSTATEHANDLER_H
#define BREWPILOT_BOILERSTATEHANDLER_H

#include "GenericStateHandler.h"
#include "Arduino.h"
#include "../../StaticConfig.h"

#define BOILER_FILL_WAIT_TIME 3000

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

    bool hasTurnedOnBoilerProbeVoltage = false;

    BoilerState internalState = BOILER_ABOVE_TARGET_AND_FILLED;

    // TODO: Maybe Implement smoothing
    bool readBoilerProbe()
    {
        if (!hasTurnedOnBoilerProbeVoltage)
        {
            digitalWrite(OUT_BOILER_VOLTAGE, HIGH);
            hasTurnedOnBoilerProbeVoltage = true;

            return internalState != BOILER_BELOW_TARGET;
        }
        else
        {
            lastCheckTime = millis();

            uint16_t rawValue = analogRead(BOILER_PROBE_PIN);

            Serial.print("Boiler Fill Check: Raw Sensor Value: ");
            Serial.println(rawValue);

            digitalWrite(OUT_BOILER_VOLTAGE, LOW);
            hasTurnedOnBoilerProbeVoltage = false;

            return rawValue <= BOILER_FILL_LEVEL;
        }
    }

public:
    explicit BoilerStateHandler(bool *isFillingBoiler) : isFillingBoiler(isFillingBoiler)
    {
        pinMode(BOILER_PROBE_PIN, INPUT);
        pinMode(OUT_BOILER_VOLTAGE, OUTPUT);

        digitalWrite(OUT_BOILER_VOLTAGE, LOW);
    };

    void handleState() override
    {
        if (internalState == BOILER_ABOVE_TARGET_BUT_FILLING)
        {
            if (millis() - lastIsAboveTargetTime > BOILER_FILL_WAIT_TIME)
            {
                bool isFilled = readBoilerProbe();

                if (!isFilled)
                {
                    Serial.println("Boiler Fill Check: Boiler is below target");
                    internalState = BOILER_BELOW_TARGET;
                }
                else
                {
                    Serial.println("Boiler Fill Check: Boiler is above target and filled");
                    internalState = BOILER_ABOVE_TARGET_AND_FILLED;
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
                case BOILER_ABOVE_TARGET_AND_FILLED:
                    if (!boilerIsFilled)
                    {
                        Serial.println("Boiler Fill Check: Boiler is below target");
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
                        Serial.println("Boiler Fill Check: Boiler is above target and filled");
                        internalState = BOILER_ABOVE_TARGET_BUT_FILLING;
                    }
                    break;
                }
            }
        }
    }
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
