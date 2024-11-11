//
// Created by Niklas Arnitz on 11.11.24.
//

#include "HardwareController.h"

void HardwareController::loop() {
    boilerController.handleState();
    groupOneController.handleState();
    groupTwoController.handleState();
    teaWaterController.handleState();
    inputHandler.readInputs();
}

void HardwareController::updateFlow(int groupId) {
    if (groupId == GROUP_ONE_ID) {
        groupOneController.updateFlow();
    } else if (groupId == GROUP_TWO_ID) {
        groupTwoController.updateFlow();
    }
}
