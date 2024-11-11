//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_TEAWATERCONTROLLER_H
#define BREWPILOT_TEAWATERCONTROLLER_H

#include "hardware/devices/Solenoid.h"
#include "../../Config.h"

class TeaWaterController {
private:
    bool isExtracting;
    long startTime;
    Solenoid waterSolenoid;

public:
    TeaWaterController() : waterSolenoid(TEA_WATER_SOLENOID_PIN, false) {
        isExtracting = false;
        startTime = 0;
    }

    void startExtracting();

    void stopExtracting();

    void getIsExtracting();

    void handleState();
};


#endif //BREWPILOT_TEAWATERCONTROLLER_H
