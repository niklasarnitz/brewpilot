#pragma once

#include "config/BuildConfig.h"
#include "config/MachineConfig.h"

#if defined(BREWPILOT_MACHINE_RANCILIO_S27)
#include "config/machines/RancilioS27.h"
constexpr MachineConfig machineConfig = rancilio_s27::config;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#include "config/machines/LaCimbaliM29Select.h"
constexpr MachineConfig machineConfig = la_cimbali_m29_select::config;
#endif

static_assert(machineConfig.groupOneActive == bool(BREWPILOT_GROUP_ONE_ACTIVE) &&
                  machineConfig.groupTwoActive == bool(BREWPILOT_GROUP_TWO_ACTIVE),
              "BREWPILOT_GROUP_*_ACTIVE in BuildConfig.h must match the machine config");
static_assert(machineConfig.groupOneActive || machineConfig.groupTwoActive, "A machine needs at least one group");
