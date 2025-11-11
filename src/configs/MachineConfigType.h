#ifndef MACHINE_CONFIG_TYPE_H
#define MACHINE_CONFIG_TYPE_H

struct RelayConfig
{
    bool pumpInverted;
    bool groupOneInverted;
    bool groupTwoInverted;
    bool boilerFillInverted;
    bool teaInverted;
};

struct MachineConfigType
{
    RelayConfig relayConfig;
};

#endif