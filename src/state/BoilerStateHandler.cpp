#include "state/BoilerStateHandler.h"

#include <Arduino.h>

#include "config/BoardPins.h"

namespace
{
constexpr uint32_t BOILER_FILL_WAIT_TIME_MS = 2000;
constexpr uint16_t BOILER_FILL_LEVEL = 1000; // Value for the LaCimbali M29 Select DT2 / R
} // namespace

BoilerStateHandler::BoilerStateHandler(bool *isFillingBoiler)
    : isFillingBoiler(isFillingBoiler)
{
    pinMode(pins::BOILER_PROBE, INPUT);
    pinMode(pins::OUT_BOILER_VOLTAGE, OUTPUT);

    digitalWrite(pins::OUT_BOILER_VOLTAGE, LOW);
}

// TODO: Maybe Implement smoothing
bool BoilerStateHandler::readBoilerProbe()
{
    if (!hasTurnedOnBoilerProbeVoltage)
    {
        digitalWrite(pins::OUT_BOILER_VOLTAGE, HIGH);
        hasTurnedOnBoilerProbeVoltage = true;

        return internalState != BoilerState::BOILER_BELOW_TARGET;
    }
    else
    {
        lastCheckTime = millis();

        uint16_t rawValue = analogRead(pins::BOILER_PROBE);
        bool full = rawValue <= BOILER_FILL_LEVEL;

        Serial.print("BoilerStateHandler: Boiler is ");
        if (!full) {
            Serial.print("not ");
        }
        Serial.print("full; Raw value: ");
        Serial.println(rawValue);

        digitalWrite(pins::OUT_BOILER_VOLTAGE, LOW);
        hasTurnedOnBoilerProbeVoltage = false;

        return full;
    }
}

void BoilerStateHandler::handleState()
{
    if (uint32_t(millis() - lastCheckTime) > BOILER_FILL_WAIT_TIME_MS || lastCheckTime == 0)
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
