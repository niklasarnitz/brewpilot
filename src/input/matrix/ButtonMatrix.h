#pragma once

#include "config/SelectedMachine.h"

// The machine is fixed at compile time, so its panel driver is used directly.
#if defined(BREWPILOT_MACHINE_RANCILIO_S27)
#include "input/matrix/RancilioS27ButtonMatrixHandler.h"
using ButtonMatrix = RancilioS27ButtonMatrixHandler;
#elif defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#include "input/matrix/LaCimbaliM29ButtonMatrixHandler.h"
using ButtonMatrix = LaCimbaliM29ButtonMatrixHandler;
#endif
