#pragma once

#include "config/MachineConfig.h"

constexpr MachineConfig laCimbaliM29SelectMachineConfig = {
    .relayConfig = {
        .pumpInverted = true,
        .groupOneInverted = true,
        .groupTwoInverted = true,
        .boilerFillInverted = true,
        .teaInverted = true,
    },
};
