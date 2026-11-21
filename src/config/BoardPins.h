#pragma once

#include <stdint.h>

// ESP32 GPIOs wired on the BrewPilot board, independent of the machine.
namespace pins
{
// Outputs
constexpr uint8_t RELAY_PUMP = 5;
constexpr uint8_t RELAY_GROUP_ONE = 4;
constexpr uint8_t RELAY_GROUP_TWO = 2;
constexpr uint8_t RELAY_BOILER_FILL = 18;
constexpr uint8_t RELAY_TEA = 15;

// Keyboard matrix connector; each machine assigns its own roles.
constexpr uint8_t FLEXIBLE_IO_1 = 36;
constexpr uint8_t FLEXIBLE_IO_2 = 12;
constexpr uint8_t FLEXIBLE_IO_3 = 39;
constexpr uint8_t FLEXIBLE_IO_4 = 27;
constexpr uint8_t FLEXIBLE_IO_5 = 26;
constexpr uint8_t FLEXIBLE_IO_6 = 25;
constexpr uint8_t FLEXIBLE_IO_7 = 33;
constexpr uint8_t FLEXIBLE_IO_8 = 32;
constexpr uint8_t FLEXIBLE_IO_9 = 19;
constexpr uint8_t FLEXIBLE_IO_10 = 13;
constexpr uint8_t FLEXIBLE_IO_11 = 14;
constexpr uint8_t FLEXIBLE_IO_12 = 23;

// Boiler probe
constexpr uint8_t BOILER_PROBE = 34;
constexpr uint8_t OUT_BOILER_VOLTAGE = 17;

// Flow meters
constexpr uint8_t GROUP_ONE_FLOW_METER = 16;
constexpr uint8_t GROUP_TWO_FLOW_METER = 35;
} // namespace pins
