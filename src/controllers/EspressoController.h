//
// Created by Niklas Arnitz on 31.10.24.
//

#ifndef BREWPILOT_ESPRESSOCONTROLLER_H
#define BREWPILOT_ESPRESSOCONTROLLER_H

#define GROUP_COUNT 2
#define TEA_WATER_COUNT 1

#include <vector>

#include "HardwareController.h"
#include "BoilerController.h"
#include "PumpController.h"
#include "GroupController.h"
#include "TeaWaterController.h"

class EspressoController : public HardwareController {
private:
    BoilerController boilerController;
    PumpController pumpController;
    std::vector<GroupController> groupControllers;
    std::vector<TeaWaterController> teaWaterControllers;

public:
    EspressoController()
            : groupControllers(GROUP_COUNT), // Initialize with GROUP_COUNT elements
              teaWaterControllers(TEA_WATER_COUNT) // Initialize with TEA_WATER_COUNT elements
    {}

    void loop() override;
};

#endif //BREWPILOT_ESPRESSOCONTROLLER_H
