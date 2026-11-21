#pragma once

#include "config/MachineType.h"

#define DEBUG true

#if defined(BREWPILOT_MACHINE_RANCILIO_S27) == defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#error "Select exactly one BrewPilot machine using a machine-specific PlatformIO environment"
#elif defined(BREWPILOT_MACHINE_RANCILIO_S27)
const MachineType HARDWARE_MODEL = MachineType::RANCILIO_S27;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
const MachineType HARDWARE_MODEL = MachineType::LA_CIMBALI_M29_SELECT;
#endif

// Override with -D flags for the installed machine.
#ifndef BREWPILOT_HAS_FLOWMETERS
#define BREWPILOT_HAS_FLOWMETERS 1
#endif

static_assert(BREWPILOT_HAS_FLOWMETERS == 0 || BREWPILOT_HAS_FLOWMETERS == 1, "Flowmeters flag must be 0 or 1");
