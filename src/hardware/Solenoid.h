//
// Created by Niklas Arnitz on 08.11.24.
//

#ifndef BREWPILOT_SOLENOIOD_H
#define BREWPILOT_SOLENOIOD_H

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
    Solenoid(int pin, bool isInverted);

    void setState(SolenoidState newState);
};

#endif //BREWPILOT_SOLENOIOD_H
