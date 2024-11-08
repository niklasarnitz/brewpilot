//
// Created by Niklas Arnitz on 08.11.24.
//

#ifndef BREWPILOT_SOLENOIDCONTROLLER_H
#define BREWPILOT_SOLENOIDCONTROLLER_H

#include "HardwareController.h"
#include "Solenoid.h"

class SolenoidController : public HardwareController {
private:
    Solenoid groupOneSolenoid;
    Solenoid groupTwoSolenoid;
    Solenoid teaWaterSolenoid;
    Solenoid boilerFillSolenoid;
public:
    void init();

    void loop() override;
};


#endif //BREWPILOT_SOLENOIDCONTROLLER_H
