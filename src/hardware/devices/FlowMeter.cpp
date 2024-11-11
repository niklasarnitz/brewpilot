//
// Created by Niklas Arnitz on 11.11.24.
//

#include "FlowMeter.h"

void FlowMeter::reset() {
    pulseCount = 0;
}

void FlowMeter::incrementPulse() {
    pulseCount++;
}

float FlowMeter::getAccumulatedVolume() {
    // TODO: Implement this
    return 0;
}
