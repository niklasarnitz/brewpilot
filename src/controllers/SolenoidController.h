//
// Created by Niklas Arnitz on 08.11.24.
//

#ifndef BREWPILOT_SOLENOIDCONTROLLER_H
#define BREWPILOT_SOLENOIDCONTROLLER_H

#include "state/State.h"
#include "HardwareController.h"
#include "hardware/Solenoid.h"

class SolenoidController : public HardwareController {
private:
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterSolenoid;
    Solenoid boilerFillSolenoid;
public:
    void init();

    void loop(State &state) override;
};


#endif //BREWPILOT_SOLENOIDCONTROLLER_H
