//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_SOLENOID_H
#define BREWPILOT_SOLENOID_H

#include "Arduino.h"

class Solenoid
{
private:
    const char *name;
    int pin;
    bool isInverted;
    bool open;

public:
    Solenoid(int pin, const char *name, bool isInverted = false)
        : pin(pin), isInverted(isInverted), open(false), name(name)
    {
        pinMode(pin, OUTPUT);

        this->setOpen(false);
    };

    void setOpen(bool newState)
    {
        if (newState != open)
        {
            Serial.printf("Setting Solenoid %s to %s\n", this->name, newState ? "open" : "closed");
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

    bool isOpen()
    {
        return open;
    }
};

#endif // BREWPILOT_SOLENOID_H
