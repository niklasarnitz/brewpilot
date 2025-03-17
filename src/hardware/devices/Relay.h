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
    Relay(int pin, bool isInverted, char *name)
    {
        this->name = name;
        this->pin = pin;
        this->isInverted = isInverted;
        this->enabled = false;

        pinMode(pin, OUTPUT);

        this->setEnabled(false);
    };

    void setEnabled(bool newState)
    {
        Serial.printf("Setting Relay %s to %s", this->name, newState ? "true" : "false");
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
