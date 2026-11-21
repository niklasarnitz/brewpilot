#pragma once

// Everything that differs between machine configurations lives in this file.
// Include config/SelectedMachine.h rather than this header.
#if !defined(BREWPILOT_MACHINE_RANCILIO_S27)
#error "This machine config belongs to another build; include config/SelectedMachine.h"
#endif

#include <stdint.h>

#include "config/BoardPins.h"
#include "config/MachineConfig.h"

namespace rancilio_s27
{
constexpr MachineConfig config = {
    .relayConfig = {
        .pumpInverted = false,
        .groupOneInverted = false,
        .groupTwoInverted = false,
        .boilerFillInverted = false,
        .teaInverted = false,
    },
    .groupOneActive = true,
    .groupTwoActive = false,
    .hasFlowMeters = true,
};

// One group panel: a single scan line drives all dose buttons.
namespace panel
{
constexpr uint8_t OUT_DOSE_BUTTONS = pins::FLEXIBLE_IO_7;
constexpr uint8_t IN_DOSE_1 = pins::FLEXIBLE_IO_4;
constexpr uint8_t IN_DOSE_2 = pins::FLEXIBLE_IO_5;
constexpr uint8_t IN_DOSE_3 = pins::FLEXIBLE_IO_6;
constexpr uint8_t IN_DOSE_4 = pins::FLEXIBLE_IO_1;
constexpr uint8_t IN_CONTINUOUS = pins::FLEXIBLE_IO_2;

// Same connector pins as the M29 tea scan line.
constexpr uint8_t OUT_TEA_WATER = pins::FLEXIBLE_IO_9;
constexpr uint8_t IN_TEA_WATER = pins::FLEXIBLE_IO_12;
} // namespace panel
} // namespace rancilio_s27
