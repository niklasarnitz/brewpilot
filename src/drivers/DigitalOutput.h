#pragma once

#include <stdint.h>

// A GPIO that drives one load, with configurable polarity and change logging.
// It starts inactive as soon as it is constructed.
class DigitalOutput
{
public:
    struct Labels
    {
        const char *device;   // e.g. "Relay"
        const char *active;   // e.g. "Enable"
        const char *inactive; // e.g. "Disable"
    };

    DigitalOutput(uint8_t pin, const char *name, bool isInverted, Labels labels);

    void setActive(bool newState);

    bool isActive() const;

private:
    uint8_t pin;
    const char *name;
    bool isInverted;
    Labels labels;
    bool active = false;
};
