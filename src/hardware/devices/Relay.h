//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_RELAY_H
#define BREWPILOT_RELAY_H

#include "Arduino.h"

class Relay
{
private:
    int pin;

    const char *name;

    bool isInverted;
    bool enabled = false;

public:
    Relay(int pin, const char *name, bool isInverted = false)
        : pin(pin), name(name), isInverted(isInverted)
    {
        pinMode(pin, OUTPUT);

        this->setEnabled(false);
    };

    void setEnabled(bool newState)
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

    bool isEnabled()
    {
        return enabled;
    }
};

#endif // BREWPILOT_RELAY_H
