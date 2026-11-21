#include "drivers/DigitalOutput.h"

#include <Arduino.h>

DigitalOutput::DigitalOutput(uint8_t pin, const char *name, bool isInverted, Labels labels)
    : pin(pin), name(name), isInverted(isInverted), labels(labels)
{
    pinMode(pin, OUTPUT);

    setActive(false);
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
