#include "Solenoid.h"

Solenoid::Solenoid(int pin, const char *name, bool isInverted) : pin(pin), name(name), isInverted(isInverted)
{
    pinMode(pin, OUTPUT);

    this->setOpen(false);
};

void Solenoid::setOpen(bool newState)
{
    if (newState != open)
    {
        Serial.printf("Solenoid: %s %s\n", newState ? "Open" : "Close", this->name);
    }

    this->open = newState;

    if (isInverted)
    {
        digitalWrite(pin, newState ? LOW : HIGH);
    }
    else
    {
        digitalWrite(pin, newState ? HIGH : LOW);
    }
}

bool Solenoid::isOpen()
{
    return open;
}
