#pragma once

// Everything that differs between machine configurations lives in this file.
// Include config/SelectedMachine.h rather than this header.
#if !defined(BREWPILOT_MACHINE_LA_CIMBALI_M29_SELECT)
#error "This machine config belongs to another build; include config/SelectedMachine.h"
#endif

// The same group values as below, for #if around code for absent groups.
#define BREWPILOT_GROUP_ONE_ACTIVE 1
#define BREWPILOT_GROUP_TWO_ACTIVE 1

#include <stdint.h>

#include "config/BoardPins.h"
#include "config/MachineConfig.h"

namespace la_cimbali_m29_select
{
constexpr MachineConfig config = {
    .relayConfig = {
        .pumpInverted = true,
        .groupOneInverted = true,
        .groupTwoInverted = true,
        .boilerFillInverted = true,
        .teaInverted = true,
    },
    .groupOneActive = BREWPILOT_GROUP_ONE_ACTIVE != 0,
    .groupTwoActive = BREWPILOT_GROUP_TWO_ACTIVE != 0,
    .hasFlowMeters = true,
};

// Two group panels scanned row by row, plus the tea button.
namespace panel
{
constexpr uint8_t IN_GROUP_TWO_BIG_DRINKS = pins::FLEXIBLE_IO_1;
constexpr uint8_t IN_GROUP_TWO_SMALL_DRINKS = pins::FLEXIBLE_IO_2;
constexpr uint8_t IN_GROUP_TWO_STOP = pins::FLEXIBLE_IO_3;
constexpr uint8_t IN_GROUP_ONE_BIG_DRINKS = pins::FLEXIBLE_IO_4;
constexpr uint8_t IN_GROUP_ONE_SMALL_DRINKS = pins::FLEXIBLE_IO_5;
constexpr uint8_t IN_GROUP_ONE_STOP = pins::FLEXIBLE_IO_6;

constexpr uint8_t OUT_GROUP_ONE_LEFT_HALF_AND_STOP = pins::FLEXIBLE_IO_7;
constexpr uint8_t OUT_GROUP_ONE_RIGHT_HALF = pins::FLEXIBLE_IO_8;

constexpr uint8_t OUT_TEA_WATER = pins::FLEXIBLE_IO_9;

constexpr uint8_t OUT_GROUP_TWO_LEFT_HALF_AND_STOP = pins::FLEXIBLE_IO_10;
constexpr uint8_t OUT_GROUP_TWO_RIGHT_HALF = pins::FLEXIBLE_IO_11;

constexpr uint8_t IN_TEA_WATER = pins::FLEXIBLE_IO_12;
} // namespace panel
} // namespace la_cimbali_m29_select
