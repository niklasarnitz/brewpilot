#pragma once

#include "config/MachineConfig.h"

constexpr MachineConfig rancilioS27MachineConfig = {
    .relayConfig = {
        .pumpInverted = false,
        .groupOneInverted = false,
        .groupTwoInverted = false,
        .boilerFillInverted = false,
        .teaInverted = false,
    },
};
