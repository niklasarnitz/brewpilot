#pragma once

#include <stdint.h>

#include "drivers/DigitalOutput.h"

class Relay
{
public:
    Relay(uint8_t pin, const char *name, bool isInverted = false);

    void setEnabled(bool newState);

    bool isEnabled() const;

private:
    DigitalOutput output;
};
