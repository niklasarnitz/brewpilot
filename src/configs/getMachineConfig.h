#ifndef GET_MACHINE_CONFIG_H
#define GET_MACHINE_CONFIG_H

#include "MachineConfig.h"
#include "../UserConfig.h"
#include "machineConfigs/LaCimbaliM29Select.h"

MachineConfig getMachineConfig()
{
    switch (HARDWARE_MODEL)
    {
    case MachineType::LA_CIMBALI_M29_SELECT:
        return laCimbaliM29SelectMachineConfig;
    }
}

#endif // GET_MACHINE_CONFIG_H