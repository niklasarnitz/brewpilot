//
// Created by Niklas Arnitz on 08.11.24.
//

#include "Solenoid.h"

void Solenoid::setState(SolenoidState newState) {
    if (newState == SOLENOID_OPEN) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}