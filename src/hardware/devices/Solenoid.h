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
public:
    Solenoid(int pin, bool isInverted) {
        this->pin = pin;
        this->isInverted = isInverted;

        pinMode(pin, OUTPUT);
    };

    void setState(bool newState);
};


#endif //BREWPILOT_SOLENOID_H
