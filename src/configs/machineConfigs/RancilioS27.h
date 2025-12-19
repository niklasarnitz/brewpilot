#ifndef RANCILIOS27_H
#define RANCILIOS27_H

#include "../MachineConfig.h"

const MachineConfig rancilioS27MachineConfig = {
    .relayConfig = {
        .pumpInverted = false,
        .groupOneInverted = false,
        .groupTwoInverted = false,
        .boilerFillInverted = false,
        .teaInverted = false,
    },
};

#endif // RANCILIOS27_H