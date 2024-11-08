//
// Created by Niklas Arnitz on 31.10.24.
//

#include "BoilerController.h"

void BoilerController::loop() {
    readBoilerSensor();
}

void BoilerController::readBoilerSensor() {
    int val = analogRead(BOILER_LEVEL_SENSOR_PIN);

    // The water level is not touching the sensor, but may be noise
    if (val > BOILER_FILL_LEVEL) fillTally++;
    else fillTally = 0;

    // Only enable fill state after a set number of "no water" reads
    if (fillTally <= FILL_TALLY_LIMIT) {
        state = BOILER_BELOW_TARGET;
    } else {
        state = BOILER_FILLED;
    }
}