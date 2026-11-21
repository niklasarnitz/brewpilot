#pragma once

#include "config/MachineConfig.h"

// Each machine configuration sets these itself; they are not build options.
#if defined(BREWPILOT_HAS_FLOWMETERS) || defined(BREWPILOT_GROUP_ONE_ACTIVE) || defined(BREWPILOT_GROUP_TWO_ACTIVE)
#error "Set flowmeters and group heads in the machine config under config/machines/, not with -D"
#endif

#if defined(BREWPILOT_MACHINE_RANCILIO_S27) == defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#error "Select exactly one BrewPilot machine using a machine-specific PlatformIO environment"
#elif defined(BREWPILOT_MACHINE_RANCILIO_S27)
#include "config/machines/RancilioS27.h"
constexpr MachineConfig machineConfig = rancilio_s27::config;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#include "config/machines/LaCimbaliM29Select.h"
constexpr MachineConfig machineConfig = la_cimbali_m29_select::config;
#endif

static_assert(machineConfig.groupOneActive || machineConfig.groupTwoActive, "A machine needs at least one group");
