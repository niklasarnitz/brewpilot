//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_SOLENOID_H
#define BREWPILOT_SOLENOID_H

#include "Arduino.h"

class Solenoid
{
private:
    int pin;

    const char *name;

    bool isInverted;
    bool open = false;

public:
    Solenoid(int pin, const char *name, bool isInverted = false);

    void setOpen(bool newState);

    bool isOpen();
};

#endif // BREWPILOT_SOLENOID_H
