#ifndef MACHINE_CONFIG_H
#define MACHINE_CONFIG_H

#include "./MachineConfigType.h"

#ifdef MACHINE_LA_CIMBALI_M29_SELECT
const MachineConfigType machineConfig = {
    .relayConfig = {
        .pumpInverted = true,
        .groupOneInverted = true,
        .groupTwoInverted = true,
        .boilerFillInverted = true,
        .teaInverted = true,
    },
};
#endif

#endif // LACIMBALIM29SELECT_H