#ifndef LACIMBALIM29SELECT_H
#define LACIMBALIM29SELECT_H

#include "../MachineConfig.h"

const MachineConfig laCimbaliM29SelectMachineConfig = {
    .relayConfig = {
        .pumpInverted = true,
        .groupOneInverted = true,
        .groupTwoInverted = true,
        .boilerFillInverted = true,
        .teaInverted = true,
    },
};

#endif // LACIMBALIM29SELECT_H