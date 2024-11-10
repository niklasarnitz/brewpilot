//
// Created by Niklas Arnitz on 31.10.24.
//

#include "EspressoController.h"

void EspressoController::loop(State &state) {
    boilerController.loop(state);
    pumpController.loop(state);
    groupOneController.loop(state);
    groupTwoController.loop(state);
    teaWaterController.loop(state);
}
