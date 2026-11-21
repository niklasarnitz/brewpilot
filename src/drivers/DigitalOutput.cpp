#include "drivers/DigitalOutput.h"

#include <Arduino.h>

DigitalOutput::DigitalOutput(uint8_t pin, const char *name, bool isInverted, Labels labels)
    : pin(pin), name(name), isInverted(isInverted), labels(labels)
{
    // Latch the inactive level before enabling the output driver, so inverted
    // loads never see their active level at boot. Arduino-ESP32 ignores
    // digitalWrite() until pinMode() has claimed the pin, so claim it as an input first.
    pinMode(pin, INPUT);
    setActive(false);
    pinMode(pin, OUTPUT);
}

void DigitalOutput::setActive(bool newState)
{
    if (newState != active)
    {
        Serial.printf("%s: %s %s\n", labels.device, newState ? labels.active : labels.inactive, name);
    }

    active = newState;

    digitalWrite(pin, newState != isInverted ? HIGH : LOW);
}

bool DigitalOutput::isActive() const
{
    return active;
}
