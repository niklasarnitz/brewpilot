//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_RELAY_H
#define BREWPILOT_RELAY_H

#include "Arduino.h"

class Relay {
private:
    int pin;
    bool isInverted;
    bool enabled;
public:
    Relay(int pin, bool isInverted) {
        this->pin = pin;
        this->isInverted = isInverted;
        this->enabled = false;

        pinMode(pin, OUTPUT);

        this->setEnabled(false);
    };

    void setEnabled(bool newState) {
        enabled = newState;

        if (isInverted) {
            digitalWrite(pin, newState ? LOW : HIGH);
        } else {
            digitalWrite(pin, newState ? HIGH : LOW);
        }
    }

    bool isEnabled() {
        return enabled;
    }
};


#endif //BREWPILOT_RELAY_H
