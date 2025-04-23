#ifndef MACHINE_CONFIG_H
#define MACHINE_CONFIG_H

#include "./hardware/MachineType.h"

struct RelayConfig
{
    bool pumpInverted;
    bool groupOneInverted;
    bool groupTwoInverted;
    bool boilerFillInverted;
    bool teaInverted;
};

struct MachineConfig
{
    RelayConfig relayConfig;
};

#endif