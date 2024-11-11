//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_GROUPHEADCONTROLLER_H
#define BREWPILOT_GROUPHEADCONTROLLER_H

#include "hardware/devices/Solenoid.h"
#include "hardware/devices/FlowMeter.h"
#include "hardware/enums/GroupHeadState.h"

class GroupHeadController {
private:
    int id;
    Solenoid solenoid;
    FlowMeter flowMeter;
    GroupHeadState state;
    double targetVolume;
public:
    GroupHeadController(int id, int solenoidPin, int flowMeterPin)
            : flowMeter(flowMeterPin), solenoid(solenoidPin, false) {
        this->id = id;
        state = GROUP_HEAD_IDLE;
        targetVolume = 0;
    }

    void handleState();

    void startExtraction(double targetVolume);

    void stopExtraction();

    void updateFlow();
};


#endif //BREWPILOT_GROUPHEADCONTROLLER_H
