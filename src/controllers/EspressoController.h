//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_ESPRESSOCONTROLLER_H
#define BREWPILOT_ESPRESSOCONTROLLER_H

#include "HardwareController.h"
#include "BoilerController.h"
#include "PumpController.h"
#include "GroupController.h"
#include "TeaWaterController.h"
#include "state/State.h"

class EspressoController : public HardwareController {
private:
    BoilerController boilerController;
    PumpController pumpController;
    GroupController groupOneController;
    GroupController groupTwoController;
    TeaWaterController teaWaterController;

public:
    void loop(State &state) override;
};

#endif //BREWPILOT_ESPRESSOCONTROLLER_H
