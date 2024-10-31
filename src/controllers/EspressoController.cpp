//
// Created by Niklas Arnitz on 31.10.24.
//

#include "EspressoController.h"

void EspressoController::loop() {
    boilerController.loop();
    pumpController.loop();

    for (auto &groupController: groupControllers) {
        groupController.loop();
    }
    for (auto &teaWaterController: teaWaterControllers) {
        teaWaterController.loop();
    }
}
