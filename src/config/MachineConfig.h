#pragma once

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
