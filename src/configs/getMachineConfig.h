#ifndef GET_MACHINE_CONFIG_H
#define GET_MACHINE_CONFIG_H

#include "MachineConfig.h"
#include "../UserConfig.h"
#include "machineConfigs/LaCimbaliM29Select.h"
#include "machineConfigs/RancilioS27.h"

MachineConfig getMachineConfig()
{
    switch (HARDWARE_MODEL)
    {
    case MachineType::LA_CIMBALI_M29_SELECT:
        return laCimbaliM29SelectMachineConfig;
    case MachineType::RANCILIO_S27:
        return rancilioS27MachineConfig;
    default:
        // Default config if unknown machine type
        return { { false, false, false, false, false } };
    }
}

#endif // GET_MACHINE_CONFIG_H