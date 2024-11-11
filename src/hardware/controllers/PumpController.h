//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_PUMPCONTROLLER_H
#define BREWPILOT_PUMPCONTROLLER_H

#include "hardware/devices/Relay.h"

class PumpController {
private:
    bool isRunning;
    Relay pumpRelay;

public:
    void start();

    void stop();

    bool getIsRunning();
};


#endif //BREWPILOT_PUMPCONTROLLER_H
