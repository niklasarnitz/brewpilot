//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_FLOWMETER_H
#define BREWPILOT_FLOWMETER_H

#include "Arduino.h"

class FlowMeter {
private:
    int pin;
    long pulseCount;
    float flowRatePerPulse;
public:
    FlowMeter(int pin) {
        this->pin = pin;
        pulseCount = 0;
        flowRatePerPulse = 0;

        pinMode(pin, INPUT);
    };

    void reset();

    void incrementPulse();

    float getAccumulatedVolume();
};


#endif //BREWPILOT_FLOWMETER_H
