//
// Created by Niklas Arnitz on 31.10.24.
//

#include "EspressoController.h"

void EspressoController::loop() {
    boilerController.loop();
    pumpController.loop();
    groupOneController.loop();
    groupTwoController.loop();
    teaWaterController.loop();
}
