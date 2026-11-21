#pragma once

#include <stdint.h>

#include "drivers/DigitalOutput.h"

class Solenoid
{
public:
    Solenoid(uint8_t pin, const char *name, bool isInverted = false);

    void setOpen(bool newState);

    bool isOpen() const;

private:
    DigitalOutput output;
};
