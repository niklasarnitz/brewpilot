//
// Created by Niklas Arnitz on 08.11.24.
//

#ifndef BREWPILOT_SOLENOIOD_H
#define BREWPILOT_SOLENOIOD_H

#include "Arduino.h"

enum SolenoidState {
    SOLENOID_OPEN,
    SOLENOID_CLOSED
};

class Solenoid {
private:
    SolenoidState state;
    int pin;
    bool isInverted;
public:
    Solenoid(int pin, bool isInverted) {
        this->pin = pin;
        this->isInverted = isInverted;
        state = SOLENOID_CLOSED;

        pinMode(pin, OUTPUT);
    };

    void setState(SolenoidState newState);
};

#endif //BREWPILOT_SOLENOIOD_H
