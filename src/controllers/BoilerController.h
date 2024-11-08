//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_BOILERCONTROLLER_H
#define BREWPILOT_BOILERCONTROLLER_H

#include "HardwareController.h"
#include "Arduino.h"
#include "Config.h"

enum BoilerState {
    BOILER_FILLED,
    BOILER_FILLING,
    BOILER_BELOW_TARGET,
    BOILER_UNKNOWN
};

class BoilerController : public HardwareController {
private:
    BoilerState state;
    int fillTally = 0;
public:
    BoilerController() {
        state = BOILER_UNKNOWN;
    }

    void loop() override;

    void readBoilerSensor();
};


#endif //BREWPILOT_BOILERCONTROLLER_H
