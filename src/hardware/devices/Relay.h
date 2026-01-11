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
    Relay(int pin, const char *name, bool isInverted = false);

    void setEnabled(bool newState);

    bool isEnabled();
};

#endif // BREWPILOT_RELAY_H
