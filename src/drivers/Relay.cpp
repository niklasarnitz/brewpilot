#include "drivers/Relay.h"

Relay::Relay(uint8_t pin, const char *name, bool isInverted)
    : output(pin, name, isInverted, {"Relay", "Enable", "Disable"}) {}

void Relay::setEnabled(bool newState)
{
    output.setActive(newState);
}

bool Relay::isEnabled() const
{
    return output.isActive();
}
