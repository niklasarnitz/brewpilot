#pragma once

#include "config/BuildConfig.h"
#include "config/MachineConfig.h"

#if defined(BREWPILOT_MACHINE_RANCILIO_S27)
#include "config/machines/RancilioS27.h"
constexpr MachineConfig machineConfig = rancilioS27MachineConfig;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#include "config/machines/LaCimbaliM29Select.h"
constexpr MachineConfig machineConfig = laCimbaliM29SelectMachineConfig;
#endif
