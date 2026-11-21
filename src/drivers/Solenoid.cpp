#include "drivers/Solenoid.h"

Solenoid::Solenoid(uint8_t pin, const char *name, bool isInverted)
    : output(pin, name, isInverted, {"Solenoid", "Open", "Close"}) {}

void Solenoid::setOpen(bool newState)
{
    output.setActive(newState);
}

bool Solenoid::isOpen() const
{
    return output.isActive();
}
