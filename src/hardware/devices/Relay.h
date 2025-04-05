//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_RELAY_H
#define BREWPILOT_RELAY_H

#include "Arduino.h"

class Relay
{
private:
    char *name;
    int pin;
    bool isInverted;
    bool enabled;

public:
    Relay(int pin, char *name, bool isInverted = false)
    {
        this->name = name;
        this->pin = pin;
        this->enabled = false;
        this->isInverted = isInverted;

        pinMode(pin, OUTPUT);

        this->setEnabled(false);
    };

    void setEnabled(bool newState)
    {
        if (newState != enabled)
        {
            Serial.printf("Setting Relay %s to %s\n", this->name, newState ? "true" : "false");
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
