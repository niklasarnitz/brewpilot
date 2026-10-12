#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "hardware/MachineType.h"

#if defined(BREWPILOT_MACHINE_RANCILIO_S27) == defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#error "Select exactly one BrewPilot machine using a machine-specific PlatformIO environment"
#elif defined(BREWPILOT_MACHINE_RANCILIO_S27)
const MachineType HARDWARE_MODEL = MachineType::RANCILIO_S27;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
const MachineType HARDWARE_MODEL = MachineType::LA_CIMBALI_M29_SELECT;
#endif

#endif
