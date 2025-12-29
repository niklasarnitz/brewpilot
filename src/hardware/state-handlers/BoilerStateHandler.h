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
            bool full = rawValue <= BOILER_FILL_LEVEL;

            Serial.print("BoilerStateHandler: Boiler is ");
            if (!full) {
                Serial.print("not ");
            }
            Serial.print("full; Raw value: ");
            Serial.println(rawValue);

            digitalWrite(OUT_BOILER_VOLTAGE, LOW);
            hasTurnedOnBoilerProbeVoltage = false;

            return full;
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
        if (millis() - lastCheckTime > BOILER_FILL_WAIT_TIME || lastCheckTime == 0)
        {
            bool boilerIsFilled = readBoilerProbe();
            if (hasTurnedOnBoilerProbeVoltage) {
                // read prepared, will actually read on next cycle
                return;
            }

            switch (internalState)
            {
            case BoilerState::BOILER_ABOVE_TARGET_AND_FILLED:
                if (!boilerIsFilled)
                {
                    Serial.println("BoilerStateHandler: Start filling");
                    *isFillingBoiler = true;
                    internalState = BoilerState::BOILER_BELOW_TARGET;
                }
                break;
            case BoilerState::BOILER_ABOVE_TARGET_BUT_FILLING:
                // intentionally ignores the read value. This state is only a delay to overfill a little.
                // if it's already underfilled again, it will start filling again on next cycle
                Serial.println("BoilerStateHandler: Stop filling");
                internalState = BoilerState::BOILER_ABOVE_TARGET_AND_FILLED;
                *isFillingBoiler = false;
                break;
            case BoilerState::BOILER_BELOW_TARGET:
                if (boilerIsFilled)
                {
                    Serial.println("BoilerStateHandler: Overfill a little");
                    internalState = BoilerState::BOILER_ABOVE_TARGET_BUT_FILLING;
                }
                break;
            }
        }
    }
};

#endif // BREWPILOT_BOILERSTATEHANDLER_H
