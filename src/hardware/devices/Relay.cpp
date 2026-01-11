#include "Relay.h"

Relay::Relay(int pin, const char *name, bool isInverted) : pin(pin), name(name), isInverted(isInverted)
{
    pinMode(pin, OUTPUT);

    this->setEnabled(false);
};

void Relay::setEnabled(bool newState)
{
    if (newState != enabled)
    {
        Serial.printf("Relay: %s %s\n", newState ? "Enable" : "Disable", this->name);
    }

    enabled = newState;

    if (isInverted)
    {
        digitalWrite(pin, newState ? LOW : HIGH);
    }
    else
    {
        digitalWrite(pin, newState ? HIGH : LOW);
    }
}

bool Relay::isEnabled()
{
    return enabled;
}
