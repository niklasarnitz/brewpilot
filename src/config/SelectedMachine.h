#pragma once

#include "config/MachineConfig.h"

// The machine config sets these; they are not build options.
#if defined(GROUP_ONE_ACTIVE) || defined(GROUP_TWO_ACTIVE) || defined(FLOWMETERS_ACTIVE)
#error "Set group heads and flowmeters in the machine config under config/machines/, not with -D"
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

// Shorthands for if constexpr; the values live only in machineConfig.
#define GROUP_ONE_ACTIVE (machineConfig.groupOneActive)
#define GROUP_TWO_ACTIVE (machineConfig.groupTwoActive)
#define FLOWMETERS_ACTIVE (machineConfig.hasFlowMeters)
