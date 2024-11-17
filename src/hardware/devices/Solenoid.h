//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_SOLENOID_H
#define BREWPILOT_SOLENOID_H

#include "Arduino.h"

class Solenoid {
private:
    int pin;
    bool isInverted;
    bool open;

public:
    Solenoid(int pin, bool isInverted) {
        this->pin = pin;
        this->isInverted = isInverted;
        this->open = false;

        pinMode(pin, OUTPUT);

        this->setOpen(false);
    };

    void setOpen(bool newState) {
        this->open = newState;

        if (isInverted) {
            digitalWrite(pin, newState ? HIGH : LOW);
        } else {
            digitalWrite(pin, newState ? LOW : HIGH);
        }
    }

    bool isOpen() {
        return open;
    }
};


#endif //BREWPILOT_SOLENOID_H
