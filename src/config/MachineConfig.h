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

    // Whether the machine has this group head. An inactive group's input,
    // state handling, valve and flow meter are not compiled into the firmware.
    bool groupOneActive;
    bool groupTwoActive;

    // Whether flow meters are installed. Without them no flow-meter interrupt
    // is attached and doses have to be stopped manually.
    bool hasFlowMeters;
};
