//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_BOILERCONTROLLER_H
#define BREWPILOT_BOILERCONTROLLER_H

#include "hardware/devices/BoilerProbe.h"
#include "hardware/devices/Solenoid.h"
#include "../../Config.h"


class BoilerController {
private:
    bool isFilled;
    BoilerProbe probe;
    Solenoid fillValve;

    void fillBoiler();

public:
    BoilerController() : fillValve(BOILER_FILL_VALVE_PIN, false) {
        isFilled = false;
        probe = BoilerProbe();
    }

    void handleState();
};


#endif //BREWPILOT_BOILERCONTROLLER_H
