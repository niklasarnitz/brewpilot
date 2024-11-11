//
// Created by Niklas Arnitz on 11.11.24.
//

#ifndef BREWPILOT_HARDWARECONTROLLER_H
#define BREWPILOT_HARDWARECONTROLLER_H

#include "BoilerController.h"
#include "PumpController.h"
#include "GroupHeadController.h"
#include "TeaWaterController.h"
#include "../handlers/InputHandler.h"
#include "../../Config.h"

class HardwareController {
private:
    BoilerController boilerController;
    PumpController pumpController{};
    GroupHeadController groupOneController;
    GroupHeadController groupTwoController;
    TeaWaterController teaWaterController;
    InputHandler inputHandler;
public:
    explicit HardwareController() : groupOneController(GROUP_ONE_ID,
                                                       GROUP_ONE_SOLENOID_PIN,
                                                       GROUP_ONE_FLOW_METER_PIN),
                                    groupTwoController(GROUP_TWO_ID,
                                                       GROUP_TWO_SOLENOID_PIN,
                                                       GROUP_TWO_FLOW_METER_PIN) {
        boilerController = BoilerController();
        pumpController = PumpController();
        teaWaterController = TeaWaterController();
        inputHandler = InputHandler();
    }

    void loop();

    void updateFlow(int groupId);
};


#endif //BREWPILOT_HARDWARECONTROLLER_H
