#pragma once

#if defined(BREWPILOT_MACHINE_RANCILIO_S27) == defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#error "Select exactly one BrewPilot machine using a machine-specific PlatformIO environment"
#elif defined(BREWPILOT_MACHINE_RANCILIO_S27)
// Group heads the machine has. Code for an absent group is not compiled;
// SelectedMachine.h checks these against machineConfig.
#define BREWPILOT_GROUP_ONE_ACTIVE 1
#define BREWPILOT_GROUP_TWO_ACTIVE 0
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#define BREWPILOT_GROUP_ONE_ACTIVE 1
#define BREWPILOT_GROUP_TWO_ACTIVE 1
#endif

// Override with -D flags for the installed machine.
#ifndef BREWPILOT_HAS_FLOWMETERS
#define BREWPILOT_HAS_FLOWMETERS 1
#endif

static_assert(BREWPILOT_HAS_FLOWMETERS == 0 || BREWPILOT_HAS_FLOWMETERS == 1, "Flowmeters flag must be 0 or 1");
